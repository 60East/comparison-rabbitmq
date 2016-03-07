# comparison-rabbitmq
This repository houses the tests and details for comparing AMPS Queues to RabbitMQ.

## Test Details

Test results were run in the 60East Engineering Lab. Both tests were run on the same system, a Supermicro SYS 1028U TNR4T+ with 2 Intel Xeon E5-2690 v3 processors (@ 12 cores each) and 128GB of memory.

For this test, we used RabbitMQ 3.5.4 and AMPS 5.0.0.0.

The AMPS test used the AMPS C++ client library, while the RabbitMQ test used the alanxz/rabbitmq-c library. For both products, the consumers were set to acknowledge consumption in batches of 80 messages. For RabbitMQ we set the prefetch count for each subscriber to 100 messages in an attempt to achieve higher performance and less time waiting for new messages to arrive. For AMPS, the maximum backlog of the subscription was set to 100 messages and we used the “proportional” distribution method was used in order to optimize for subscriber efficiency. The proportional method is the most compute-intensive distribution method for the server, since it balances delivery to subscribers based on current subscriber capacity. All tests were run over the loopback interface, with both publishers and subscribers on the same system. For this test, we configured AMPS to use a 100MB size for journal files.

On the test system, the only available spinning hard drive was the host’s boot volume, a Seagate ST9250610NS. This enterprise-class hard drive is configured with the operating system’s defaults and thus could not be optimally configured for this test. For best AMPS journal performance on spinning disk we suggest mounting the filesystem with "noatime" and "nodiratime", and reserving the entire drive for the AMPS workload, none of which could be configured for this test. We plan to test with a dedicated hard drive of the same model, optimized for data storage rather than use as a boot volume. If the numbers are significantly different, we plan to update this paper with the revised numbers to more accurately reflect performance on a production system.

The Intel SSD DC P3700 was dedicated to this testing (AMPS or RabbitMQ) with no other workload. This drive is configured with the ext4 file system, with a number of optimizations for best performance in a messaging workload. These optimizations are: disabling the "has journal" option via tune2fs; enabling "journal data writeback" via tune2fs; and mounting the filesystem with the "noatime" and "nodiratime" options.

# Running the Tests

This repository contains the source for the publishers and consumers in the test and a simple harness for each test.

To run the tests:

## RabbitMQ

We ran our tests using RabbitMQ 3.5.4, installed through our distribution's package manager.

To run the test, we:

* Set the RabbitMQ configuration so that the `MNESIA_BASE` directory is located on the device you want to use for the persistent queue. Typically, the configuration file is located at `/etc/rabbitmq/rabbitmq-env.conf`, although your distribution may use a different location.

* Downloaded and built the RabbitMQ C client from [the github repository](https://github.com/alanxz/rabbitmq-c).

* Built the test in the `rabbitmq` directory. By default, the Makefile for the test looks for the C client in the home directory of the current user. To use a client distribution located elsewhere, you can set `RABBIT_DIR` to the location of the client.

* Ran the test with the provided shell script (`runone.sh`). The script takes the following arguments: 
   * Number of publishers
   * Number of subscribers
   * Message count (we used 2 million messages)
   * Size of messages, in bytes

  For example, `./runone.sh 1 1 200000 512`

In between runs, we recommend shutting down RabbitMQ and clearing the storage directory. To stop RabbitMQ, use `/sbin/service rabbitmq-server stop` or the equivalent on your distribution. To start RabbitMQ, use `/sbin/service rabbitmq-server start`, or the equivalent on your distribution.


## AMPS

We ran our test using AMPS 5.0.0.0, downloaded from the 60East [evaluation page](http://www.crankuptheamps.com/evaluate) and unpacked into the home directory of the test user.

* Downloaded the AMPS C++ Client version 5.0.0.0, available [here](http://devnull.crankuptheamps.com/releases/amps/clients/amps-c++-client-develop-Linux.tar.gz).

* Built the test in the amps directory. By default, the makefile for the test looks for the C client in the `clients/cpp` directory beneath the home directory of the current user. Otherwise you can set `AMPS_CLIENT` environment variable to the location of the AMPS client distribution.

* Set the `STORAGE_DIR` environment variable to the device where we want AMPS to persist the queue (that is, where AMPS stores the transaction log).

* Set the `AMPS_DIR` environment variable to the location where we extracted the AMPS server. Ran the test with the provided shell script (`runone.sh`). The script takes the following arguments: 
   * Number of publishers
   * Number of subscribers
   * Message count (we used 2 million messages)
   * Size of messages, in bytes

  For example, `./runone.sh 1 1 200000 512`

In between runs, we recommend shutting down AMPS and clearing the storage directory.


