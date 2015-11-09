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
        private IPropertySet settings = ApplicationData.Current.LocalSettings.Values;

        public MainPage()
        {
            this.InitializeComponent();

            this.NavigationCacheMode = NavigationCacheMode.Required;

            if (settings.ContainsKey("Host"))
                host.Text = settings["Host"].ToString();

            if (settings.ContainsKey("Port"))
                port.Text = settings["Port"].ToString();
            else
                settings["Port"] = port.Text = "2711";
        }

        private void host_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (settings.ContainsKey("Host"))
                settings["Host"] = host.Text;
            else
                settings.Add("Host", host.Text);
        }

        private void port_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (settings.ContainsKey("Port"))
                settings["Port"] = port.Text;
            else
                settings.Add("Port", port.Text);
        }
    }
}
