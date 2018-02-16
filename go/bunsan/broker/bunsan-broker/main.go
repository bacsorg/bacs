package main

import (
	"flag"
	"io/ioutil"
	"log"
	"os"
	"os/signal"
	"strings"
	"syscall"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/service"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker"
	"github.com/bunsanorg/pm/go/bunsan/pm"
)

var url = flag.String("url", "", "Example: amqp://guest:guest@localhost/")
var jobs = flag.Int("jobs", 1, "Number of parallel jobs")
var constraints = flag.String("constraints", "", ",-separated list")
var repositoryConfig = flag.String("repository-config", "",
	"bunsan::pm configuration file")
var tmpdir = flag.String("tmpdir", "", "Temporary directory for workers")

func abortOnSignal(
	reader service.RequestReader,
	workerPool worker.WorkerPool) {

	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)
	go func() {
		sig := <-sigCh
		log.Printf("Received signal %q, aborting", sig)
		reader.Close()
		workerPool.Abort()
	}()
}

// scope
func run() error {
	constraints := broker.Constraints{
		Resource: strings.Split(*constraints, ","),
	}

	repository, err := pm.NewRepository(*repositoryConfig)
	if err != nil {
		return err
	}
	defer repository.Close()

	workerPool := worker.NewWorkerPool()
	for i := 0; i < *jobs; i++ {
		dir, err := ioutil.TempDir(*tmpdir, "")
		if err != nil {
			return err
		}
		defer os.RemoveAll(dir)
		worker := worker.NewWorker(repository, dir)
		workerPool.Add(worker)
	}

	reader := service.NewRequestReader(*url)
	defer reader.Close()
	abortOnSignal(reader, workerPool)
	tasks, err := reader.Read(constraints, *jobs)
	if err != nil {
		return err
	}
	log.Print("Processing tasks...")
	return workerPool.DoAll(tasks)
}

func main() {
	flag.Parse()
	if *url == "" {
		log.Fatal("Must set -url")
	}
	if *constraints == "" {
		log.Fatal("Must set -constraints")
	}
	if *repositoryConfig == "" {
		log.Fatal("Must set -repository-config")
	}

	err := run()
	if err != nil {
		log.Fatal(err)
	}
}
