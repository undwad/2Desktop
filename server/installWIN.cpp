#include <QList>
#include <QApplication>
#include <QThread>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QElapsedTimer>
#include <QDebug>
#include <QSettings>

#include <windows.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <shellapi.h>

#include "install.h"

QString perMachineProgramDir()
{
    PWSTR ptr = nullptr;
    if(S_OK == SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_CREATE, 0, &ptr))
    {
        QString result = QString::fromWCharArray(ptr) + QDir::separator() + QApplication::applicationName();
        QDir dir(result);
        if(!dir.exists()) dir.mkpath(result);
        return result;
    }
    qFatal("perMachineProgramDir failed");
}

bool isRunningUnderAdmin()
{
    BOOL fReturn = FALSE;
    DWORD dwStatus;
    DWORD dwAccessMask;
    DWORD dwAccessDesired;
    DWORD dwACLSize;
    DWORD dwStructureSize = sizeof(PRIVILEGE_SET);
    PACL pACL = NULL;
    PSID psidAdmin = NULL;

    HANDLE hToken = NULL;
    HANDLE hImpersonationToken = NULL;

    PRIVILEGE_SET ps;
    GENERIC_MAPPING GenericMapping;

    PSECURITY_DESCRIPTOR psdAdmin = NULL;
    SID_IDENTIFIER_AUTHORITY SystemSidAuthority = SECURITY_NT_AUTHORITY;

    const DWORD ACCESS_READ = 1;
    const DWORD ACCESS_WRITE = 2;

    __try
    {
       if (!OpenThreadToken(GetCurrentThread(), TOKEN_DUPLICATE|TOKEN_QUERY, TRUE, &hToken))
       {
          if (GetLastError() != ERROR_NO_TOKEN) __leave;

          if (!OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE|TOKEN_QUERY, &hToken)) __leave;
       }

       if (!DuplicateToken (hToken, SecurityImpersonation, &hImpersonationToken)) __leave;

       if (!AllocateAndInitializeSid(&SystemSidAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &psidAdmin)) __leave;

       psdAdmin = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
       if (psdAdmin == NULL) __leave;

       if (!InitializeSecurityDescriptor(psdAdmin, SECURITY_DESCRIPTOR_REVISION)) __leave;

       dwACLSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psidAdmin) - sizeof(DWORD);

       pACL = (PACL)LocalAlloc(LPTR, dwACLSize);
       if (pACL == NULL) __leave;

       if (!InitializeAcl(pACL, dwACLSize, ACL_REVISION2)) __leave;

       dwAccessMask= ACCESS_READ | ACCESS_WRITE;

       if (!AddAccessAllowedAce(pACL, ACL_REVISION2, dwAccessMask, psidAdmin)) __leave;

       if (!SetSecurityDescriptorDacl(psdAdmin, TRUE, pACL, FALSE)) __leave;

       SetSecurityDescriptorGroup(psdAdmin, psidAdmin, FALSE);
       SetSecurityDescriptorOwner(psdAdmin, psidAdmin, FALSE);

       if (!IsValidSecurityDescriptor(psdAdmin)) __leave;

       dwAccessDesired = ACCESS_READ;

       GenericMapping.GenericRead = ACCESS_READ;
       GenericMapping.GenericWrite = ACCESS_WRITE;
       GenericMapping.GenericExecute = 0;
       GenericMapping.GenericAll = ACCESS_READ | ACCESS_WRITE;

       if (!AccessCheck(psdAdmin, hImpersonationToken, dwAccessDesired, &GenericMapping, &ps, &dwStructureSize, &dwStatus, &fReturn))
       {
          fReturn = FALSE;
          __leave;
       }
    }
    __finally
    {
       if (pACL) LocalFree(pACL);
       if (psdAdmin) LocalFree(psdAdmin);
       if (psidAdmin) FreeSid(psidAdmin);
       if (hImpersonationToken) CloseHandle (hImpersonationToken);
       if (hToken) CloseHandle (hToken);
    }

    return TRUE == fReturn;
}

bool obtainPrivilege(const wchar_t* name)
{
    bool result = false;

    HANDLE token;
    if(TRUE == OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
    {
        LUID luid;
        if(TRUE == LookupPrivilegeValue(nullptr, name, &luid))
        {
            TOKEN_PRIVILEGES privileges = {1, { { luid, SE_PRIVILEGE_ENABLED } } };
            DWORD len;
            result = TRUE == AdjustTokenPrivileges(token, FALSE, &privileges, sizeof(privileges), nullptr, &len);
        }
        CloseHandle(token);
    }
    return result;
}

void killProcess(const QString& exe)
{
    QFileInfo exeInfo(exe);
    obtainPrivilege(SE_DEBUG_NAME);
    HANDLE process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(INVALID_HANDLE_VALUE != process_snapshot)
    {
        PROCESSENTRY32 process_entry = { sizeof(PROCESSENTRY32), 0 };
        if(Process32First(process_snapshot, &process_entry))
            do
            {
                if(HANDLE module_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_entry.th32ProcessID))
                {
                    MODULEENTRY32 module_entry = { sizeof(MODULEENTRY32), 0 };
                    if(Module32First(module_snapshot, &module_entry))
                        if(QFileInfo(QString::fromWCharArray(module_entry.szExePath)) == exeInfo)
                        {
                            qDebug() << "killing" << process_entry.th32ProcessID;
                            if(HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, process_entry.th32ProcessID))
                            {
                                TerminateProcess(process, 0);
                                CloseHandle(process);
                            }

                        }
                    CloseHandle(module_snapshot);
                }
            }
            while(Process32Next(process_snapshot, &process_entry));
        CloseHandle(process_snapshot);
    }
}

bool createExeLink(const QString& exe, const QString& args, const QString& dir, const QString& desc, const QString& target)
{
    bool result = false;
    IShellLink* shellLink = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&shellLink)))
    {
        shellLink->SetPath(exe.toStdWString().c_str());
        shellLink->SetArguments(args.toStdWString().c_str());
        shellLink->SetWorkingDirectory(dir.toStdWString().c_str());
        shellLink->SetDescription(desc.toStdWString().c_str());
        IPersistFile* persistFile = nullptr;
        if (SUCCEEDED(shellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&persistFile)))
        {
            result = SUCCEEDED(persistFile->Save(target.toStdWString().c_str(), TRUE));
            persistFile->Release();
        }
        shellLink->Release();
    }
    return result;
}

bool executeCommand(const QString& exe, const QString& args, const QString& dir)
{
    return (int)ShellExecute(0, nullptr, exe.toStdWString().c_str(), args.toStdWString().c_str(), dir.toStdWString().c_str(), SW_SHOWNORMAL) > 32;
}

bool executeCommandUnderAdmin(const QString& exe, const QString& args, const QString& dir)
{
    return (int)ShellExecute(0, L"runas", exe.toStdWString().c_str(), args.toStdWString().c_str(), dir.toStdWString().c_str(), SW_SHOWNORMAL) > 32;
}

void registerAutoRun(const QString& exe, const QString& args)
{
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::Format::NativeFormat);
    registry.setValue(QApplication::applicationName(), QVariant(QString("\"%1\" %2").arg(exe).arg(args)));
    registry.sync();
}
