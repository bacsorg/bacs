using CommandLine;
using Bunsan.Broker;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker.ClientExample
{
    class Options
    {
        [Option("identifier", DefaultValue = "test-client", HelpText = "Identifier")]
        public string Identifier { get; set; }

        [Option("host", HelpText = "Broker host")]
        public string Host { get; set; }

        [Option("port", HelpText = "Broker port")]
        public int Port { get; set; }

        [Option("virtual-host", HelpText = "Virtual host of broker")]
        public string VirtualHost { get; set; }

        [Option("username", HelpText = "Username")]
        public string UserName { get; set; }

        [Option("password", HelpText = "Password")]
        public string Password { get; set; }

        [Option("resource", DefaultValue = "resource", HelpText = "Task resource")]
        public string Resource { get; set; }

        [Option("type", DefaultValue = "task-type", HelpText = "Task type")]
        public string Type { get; set; }

        [Option("package", DefaultValue = "package", HelpText = "Package")]
        public string Package { get; set; }

        [Option("number", DefaultValue = 10, HelpText = "Number of messages to send")]
        public int NumberOfMessages { get; set; }

        [Option("interval", DefaultValue = 5000, HelpText = "Interval between messages in ms")]
        public int Interval { get; set; }

        [HelpOption(HelpText = "Display this help screen.")]
        public string GetUsage()
        {
            var usage = new StringBuilder();
            usage.AppendLine("Bunsan.Broker.Client example application");
            return usage.ToString();
        }
    }

    class Program
    {
        static int Main(string[] args)
        {
            var options = new Options();
            if (!Parser.Default.ParseArguments(args, options))
            {
                Console.WriteLine("Bad arguments!");
                return 1;
            }
            var connection_parameters = new ConnectionParameters();
            if (options.Identifier != null) connection_parameters.Identifier = options.Identifier;
            if (options.Host != null) connection_parameters.Host = options.Host;
            if (options.Port != 0) connection_parameters.Port = options.Port;
            if (options.VirtualHost != null) connection_parameters.VirtualHost = options.VirtualHost;
            if (options.UserName != null || options.Password != null) connection_parameters.Credentials = new Credentials();
            if (options.UserName != null) connection_parameters.Credentials.UserName = options.UserName;
            if (options.Password != null) connection_parameters.Credentials.Password = options.Password;
            var client = new Client(connection_parameters);
            client.Listen((id, status) => {
                Console.WriteLine("Got status: id = ", id, ", status = ", status);
            }, (id, result) => {
                Console.WriteLine("Got result: id = ", id, ", status = ", result);
            });
            for (int id = 0; id < options.NumberOfMessages; ++id)
            {
                client.Send(new Constraints() { Resource = new string[] { options.Resource } }, id.ToString(), new Task()
                {
                    Type = options.Type,
                    Package = options.Package,
                    Data = System.Text.Encoding.UTF8.GetBytes("hello, world"),
                });
                System.Threading.Thread.Sleep(options.Interval);
            }
            client.Terminate();
            return 0;
        }
    }
}
