using RabbitMQ.Client;
using RabbitMQ.Client.Exceptions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    public class Connection : IDisposable
    {
        private readonly ConnectionFactory connection_factory;
        protected bool running = true;
        private IConnection connection;
        protected IModel channel;

        protected object Lock { get { return connection_factory; } }

        public IModel Channel
        {
            get
            {
                lock (Lock)
                {
                    reconnect();
                    return channel;
                }
            }
        }

        public bool IsRunning
        {
            get
            {
                lock (Lock)
                {
                    return running;
                }
            }
        }

        public bool IsClosed { get { return !IsRunning; } }

        public event Action<IModel> OnConnect = delegate { };
        
        public Connection(ConnectionParameters parameters)
        {
            connection_factory = new ConnectionFactory();
            if (parameters.Host != null) connection_factory.HostName = parameters.Host;
            if (parameters.Port != 0) connection_factory.Port = parameters.Port;
            if (parameters.VirtualHost != null) connection_factory.VirtualHost = parameters.VirtualHost;
            if (parameters.Credentials != null)
            {
                var credentials = parameters.Credentials;
                if (credentials.UserName != null) connection_factory.UserName = credentials.UserName;
                if (credentials.Password != null) connection_factory.Password = credentials.Password;
            }
        }

        public void Close()
        {
            lock (Lock)
            {
                running = false;
                if (connection != null && connection.IsOpen)
                    connection.Close();
            }
        }

        private void connect()
        {
            connection = connection_factory.CreateConnection();
            channel = connection.CreateModel();
            OnConnect(channel);
        }

        private void reconnect()
        {
            if (!running) return;
            while (connection == null || !connection.IsOpen)
            {
                try
                {
                    connect();
                }
                catch (BrokerUnreachableException)
                {
                    Thread.Sleep(5000);
                }
            }
        }

        // Connect if necessary, i.e. not closed and not connected
        public void Reconnect()
        {
            lock (Lock)
            {
                reconnect();
            }
        }

        public void Dispose()
        {
            Close();
        }
    }
}
