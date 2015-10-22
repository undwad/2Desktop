using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.ApplicationModel.Activation;
using Windows.Storage;

namespace ToDesktop
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

            this.NavigationCacheMode = NavigationCacheMode.Required;

            if (ApplicationData.Current.LocalSettings.Values.ContainsKey("Host"))
                host.Text = ApplicationData.Current.LocalSettings.Values["Host"].ToString();

            if (ApplicationData.Current.LocalSettings.Values.ContainsKey("Port"))
                port.Text = ApplicationData.Current.LocalSettings.Values["Port"].ToString();
        }

        private void host_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (ApplicationData.Current.LocalSettings.Values.ContainsKey("Host")) 
                ApplicationData.Current.LocalSettings.Values["Host"] = host.Text;
            else 
                ApplicationData.Current.LocalSettings.Values.Add("Host", host.Text);
        }

        private void port_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (ApplicationData.Current.LocalSettings.Values.ContainsKey("Port"))
                ApplicationData.Current.LocalSettings.Values["Port"] = port.Text;
            else
                ApplicationData.Current.LocalSettings.Values.Add("Port", port.Text);
        }
    }
}
