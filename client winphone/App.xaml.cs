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
using Windows.Web;
using System.Text;

namespace ToDesktop
{
    public sealed partial class App : Application
    {
        private IPropertySet settings = ApplicationData.Current.LocalSettings.Values;

        public App() { this.InitializeComponent(); }

        void show()
        {
            var frame = new Frame();
            frame.Navigate(typeof(MainPage));
            Window.Current.Content = frame;
            Window.Current.Activate();
        }

        protected override void OnLaunched(LaunchActivatedEventArgs args) { show(); }

        protected override async void OnShareTargetActivated(ShareTargetActivatedEventArgs args)
        {
            if (args.ShareOperation.Data.Contains(StandardDataFormats.WebLink))
            {
                Uri uri = await args.ShareOperation.Data.GetWebLinkAsync();
                if (null != uri)
                try
                {
                    using (var socket = new MessageWebSocket())
                    {
                        socket.Control.MessageType = SocketMessageType.Utf8;
                        await socket.ConnectAsync(new Uri("ws://" + settings["Host"].ToString() + ":" + settings["Port"].ToString()));
                        using (var writer = new DataWriter(socket.OutputStream))
                        {
                            writer.WriteString(uri.AbsoluteUri);
                            await writer.StoreAsync();
                            args.ShareOperation.ReportCompleted();
                        }
                    }
                }
                catch
                {
                    show();
                }
            }
        }
    }
}