#pragma once

#include <QString>

QString perMachineProgramDir();
bool isRunningUnderAdmin();
bool obtainPrivilege(const wchar_t* name);
void killProcess(const QString& exe);
bool createExeLink(const QString& exe, const QString& args, const QString& dir, const QString& desc, const QString& target);
bool executeCommand(const QString& exe, const QString& args, const QString& dir);
bool executeCommandUnderAdmin(const QString& exe, const QString& args, const QString& dir);
void registerAutoRun(const QString& exe);

int install(const QString& exeArgs);
