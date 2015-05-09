using Bunsan.Broker;
using NDesk.Options;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker.ClientExample
{
    class Program
    {
        static int Main(string[] args)
        {
            var connection_parameters = new ConnectionParameters();
            var constraints = new Constraints() { Resource = new string[] { "resource" } };
            var task = new Task();
            int interval = 5000;
            int number_of_messages = 10;
            Action ensure_credentials = () => { if (connection_parameters.Credentials == null) connection_parameters.Credentials = new Credentials(); };
            var options = new OptionSet()
            {
                {"identifier=", "", identifier => connection_parameters.Identifier = identifier},
                {"host=", "", host => connection_parameters.Host = host},
                {"port=", "", port => connection_parameters.Port = int.Parse(port)},
                {"virtual-host=", "", vhost => connection_parameters.VirtualHost = vhost},
                {"username=", "", username => { ensure_credentials(); connection_parameters.Credentials.UserName = username; } },
                {"password=", "", password => { ensure_credentials(); connection_parameters.Credentials.Password = password; } },
                {"resource=", "", resource => constraints.Resource[0] = resource},
                {"worker=", "", worker => task.Worker = worker},
                {"package=", "", package => task.Package = package},
                {"data=", "", data => task.Data = System.Text.Encoding.UTF8.GetBytes(data)},
                {"interval=", "", interval_str => interval = int.Parse(interval_str)},
                {"number=", "", number => number_of_messages = int.Parse(number)},
            };
            try
            {
                options.Parse(args);
            }
            catch (OptionException e)
            {
                Console.WriteLine("Argument error: ", e.Message);
            }
            var client = new Client(connection_parameters);
            client.Listen((id, status) =>
            {
                Console.WriteLine("Got status: id = {0}, status = [{1}, {2}]", id, status.Code, status.Reason);
            }, (id, result) =>
            {
                Console.WriteLine("Got result: id = {0}, result = [{1}, {2}]", id, result.Status, result.Reason);
            }, (id, error) =>
            {
                Console.WriteLine("Got error: id = {0}, error = {1}", id, error);
            });
            for (int id = 0; id < number_of_messages; ++id)
            {
                client.Send(constraints, id.ToString(), task);
                System.Threading.Thread.Sleep(interval);
            }
            client.Close();
            return 0;
        }
    }
}
