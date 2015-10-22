using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;
using Windows.ApplicationModel.DataTransfer;
using Windows.ApplicationModel.DataTransfer.ShareTarget;
using Windows.Networking.Sockets;
using Windows.Networking;
using Windows.Storage;
using Windows.Storage.Streams;
using System.Text;

namespace ToDesktop
{
    public sealed partial class App : Application
    {
        public App() { this.InitializeComponent(); }


        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            var frame = new Frame();
            frame.Navigate(typeof(MainPage));
            Window.Current.Content = frame;
            Window.Current.Activate();
        }

        protected override async void OnShareTargetActivated(ShareTargetActivatedEventArgs args)
        {
            if (ApplicationData.Current.LocalSettings.Values.ContainsKey("Host"))
                if (ApplicationData.Current.LocalSettings.Values.ContainsKey("Port"))
                    if (args.ShareOperation.Data.Contains(StandardDataFormats.WebLink))
                    {
                        Uri uri = await args.ShareOperation.Data.GetWebLinkAsync();
                        if (null != uri)
                        {
                            using (var socket = new DatagramSocket())
                            using
                            (
                                var stream = await socket.GetOutputStreamAsync
                                (
                                    new HostName(ApplicationData.Current.LocalSettings.Values["Host"].ToString()),
                                    ApplicationData.Current.LocalSettings.Values["Port"].ToString()
                                )
                            )
                            {
                                await stream.WriteAsync(Encoding.UTF8.GetBytes(uri.AbsoluteUri).AsBuffer());
                                args.ShareOperation.ReportCompleted();
                            }
                        }
                    }
        }
    }
}