using ProtoBuf;
using RabbitMQ.Client;
using RabbitMQ.Client.Events;
using RabbitMQ.Client.Exceptions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Bunsan.Broker
{
    public abstract class ClientBase : IDisposable
    {
        public string StatusQueue { get; set; }
        public string ResultQueue { get; set; }
        public string ErrorQueue { get; set; }
        private readonly ConnectionFactory connection_factory;
        protected bool running = true;
        private IConnection connection;
        protected IModel channel;

        protected object Lock { get { return connection_factory; } }

        public ClientBase(ConnectionParameters parameters)
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
            if (string.IsNullOrEmpty(parameters.Identifier)) throw new ArgumentException("Expected non-empty Identifier");
            StatusQueue = "client." + parameters.Identifier + ".status";
            ResultQueue = "client." + parameters.Identifier + ".result";
            ErrorQueue = "client." + parameters.Identifier + ".error";
        }

        public delegate void StatusCallback(string id, Status status);
        public delegate void ResultCallback(string id, Result result);
        public delegate void ErrorCallback(string id, string error);

        protected void reconnect()
        {
            lock (Lock)
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
        }

        private void connect()
        {
            connection = connection_factory.CreateConnection();
            channel = connection.CreateModel();
            bind();
        }

        protected abstract void bind();

        public void Close()
        {
            lock (Lock)
            {
                running = false;
                if (connection != null && connection.IsOpen)
                    connection.Close();
            }
        }

        public void Dispose()
        {
            Close();
        }
    }
}
