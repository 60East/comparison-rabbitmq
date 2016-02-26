# comparison-rabbitmq
This repository houses the tests and details for comparing AMPS Queues to RabbitMQ.

> This is a work in progress -- stay tuned for updates!


## Test Details

Test results were run in the 60East Engineering Lab. Both tests were run on the same system, a Supermicro SYS 1028U TNR4T+ with 2 Intel Xeon E5-2690 v3 processors (@ 12 cores each) and 128GB of memory.

The AMPS test used the AMPS C++ client library, while the RabbitMQ test used the alanxz/rabbitmq-c library. For both products, the consumers were set to acknowledge consumption in batches of 80 messages. For RabbitMQ we set the prefetch count for each subscriber to 100 messages in an attempt to achieve higher performance and less time waiting for new messages to arrive. For AMPS, the maximum backlog of the subscription was set to 100 messages and we used the “proportional” distribution method was used in order to optimize for subscriber efficiency. The proportional method is the most compute-intensive distribution method for the server, since it balances delivery to subscribers based on current subscriber capacity. All tests were run over the loopback interface, with both publishers and subscribers on the same system. For this test, we configured AMPS to use a 100MB size for journal files.

On the test system, the only available spinning hard drive was the host’s boot volume, a Seagate ST9250610NS. This enterprise-class hard drive is configured with the operating system’s defaults and thus could not be optimally configured for this test. For best AMPS journal performance on spinning disk we suggest mounting the filesystem with “noatime” and “nodiratime”, and reserving the entire drive for the AMPS workload, none of which could be configured for this test. We plan to test with a dedicated hard drive of the same model, optimized for data storage rather than use as a boot volume. If the numbers are significantly different, we plan to update this paper with the revised numbers to more accurately reflect performance on a production system.

The Intel SSD DC P3700 was dedicated to this testing (AMPS or RabbitMQ) with no other workload. This drive is configured with the ext4 file system, with a number of optimizations for best performance in a messaging workload. These optimizations are: disabling the "has journal" option via tune2fs; enabling "journal data writeback" via tune2fs; and mounting the filesystem with the "noatime" and "nodiratime" options.

