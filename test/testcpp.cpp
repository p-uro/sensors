
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <signal.h>
#include "test.h"
#include <mapper/mapper_cpp.h>
#include "bitalino.h"
#include <sys/select.h>
#include <thread>
#include <chrono>

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

using namespace mapper;

int received = 0;
int done = 0;
int val = 100;
int verbose = 1;
int terminate = 0;
int period = 100;

class out_stream : public std::ostream
{
public:
    out_stream() : std::ostream(&buf) {}

private:
    class null_out_buf : public std::streambuf
    {
    public:
        virtual std::streamsize xsputn(const char *s, std::streamsize n)
        {
            return n;
        }
        virtual int overflow(int c)
        {
            return 1;
        }
    };
    null_out_buf buf;
};
out_stream null_out;

void instance_handler_in(Signal::Instance &&si, Signal::Event event, int length,
                         Type type, const void *value, Time &&t)
{
    ++received;
    if (verbose)
    {
        std::cout << "\t\t\t\t\t   | --> signal update:  input signal: " << si.signal()[Property::NAME] << ", new value: ";
    }

    if (!value)
    {
        if (verbose)
            std::cout << " ––––––––" << std::endl;
        si.release();
        return;
    }
    else if (!verbose)
        return;

    switch (type)
    {
    case Type::INT32:
    {
        int *v = (int *)value;
        for (int i = 0; i < length; i++)
        {
            std::cout << " " << v[i] << std::endl;
        }
        break;
    }
    default:
        break;
    }
    std::cout << std::endl;
}

void instance_handler_out(Signal::Instance &&si, Signal::Event event, int length,
                          Type type, const void *value, Time &&t)
{
    ++received;
    if (verbose)
    {
        std::cout << "\t\t\t\t\t   | --> signal update: output signal: " << si.signal()[Property::NAME] << ", new value: ";
    }

    if (!value)
    {
        if (verbose)
            std::cout << " ––––––––" << std::endl;
        si.release();
        return;
    }
    else if (!verbose)
        return;

    switch (type)
    {
    case Type::INT32:
    {
        int *v = (int *)value;
        for (int i = 0; i < length; i++)
        {
            std::cout << " " << v[i] << std::endl;
        }
        break;
    }
    default:
        break;
    }
    std::cout << std::endl;
}

void ctrlc(int sig)
{
    done = 1;
}

bool keypressed(void)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);

    timeval readtimeout;
    readtimeout.tv_sec = 0;
    readtimeout.tv_usec = 0;

    return (select(FD_SETSIZE, &readfds, NULL, NULL, &readtimeout) == 1);
}

// void p()
// {
//     std::cout << "Hello, World\n";
//     std::cin.ignore();
// }

// void randomizer()
// {
//     // int count = 0;
//     while (val < 110)
//     {
//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//         val++;
//         // count++;
//     }
// }
void polling_data()
{
    try
    {
        // uncomment this block to search for Bluetooth devices (Windows and Linux)
        /*
      BITalino::VDevInfo devs = BITalino::find();   
      for(int i = 0; i < devs.size(); i++)
      	printf("%s - %s\n", devs[i].macAddr.c_str(), devs[i].name.c_str());
      return 0;
      */

        puts("Connecting to device...");

        BITalino dev("/dev/tty.BITalino-1E-10-DevB"); // device MAC address (Windows and Linux)

        puts("Connected to device. Press Enter to exit.");

        std::string ver = dev.version(); // get device version string
        printf("BITalino version: %s\n", ver.c_str());

        dev.battery(10); // set battery threshold (optional)

        dev.start(1000, {0, 1, 2, 3, 4, 5}); // start acquisition of all channels at 1000 Hz
        // use block below if your compiler doesn't support vector initializer lists

        // BITalino::Vint chans;
        // chans.push_back(0);
        // chans.push_back(1);
        // chans.push_back(2);
        // chans.push_back(3);
        // chans.push_back(4);
        // chans.push_back(5);
        // dev.start(1000, chans);

        BITalino::VFrame frames(100); // initialize the frames vector with 100 frames
        do
        {
            dev.read(frames);                     // get 100 frames from device
            const BITalino::Frame &f = frames[0]; // get a reference to the first frame of each 100 frames block
            val = f.analog[0];
            // out << val << std::endl;
            // printf("%d : %d %d %d %d ; %d %d %d %d %d %d\n", // dump the first frame
            //        f.seq,
            //        f.digital[0], f.digital[1], f.digital[2], f.digital[3],
            //        f.analog[0], f.analog[1], f.analog[2], f.analog[3], f.analog[4], f.analog[5]);

        } while (!keypressed()); // until a key is pressed

        dev.stop(); // stop acquisition
        // return val;
    } // dev is destroyed here (it goes out of scope)
    catch (BITalino::Exception &e)
    {
        printf("BITalino exception: %s\n", e.getDescription());
    }
}

int main(int argc, char **argv)
{
    int i = 0, j, result = 0;
    char *iface = 0;
    // std::thread thread_object1(randomizer);
    std::thread thread_object2(polling_data);
    // process flags for -v verbose, -t terminate, -h help
    for (i = 1; i < argc; i++)
    {
        if (argv[i] && argv[i][0] == '-')
        {
            int len = (int)strlen(argv[i]);
            for (j = 1; j < len; j++)
            {
                switch (argv[i][j])
                {
                case 'h':
                    printf("testcpp.cpp: possible arguments "
                           "-q quiet (suppress output), "
                           "-t terminate automatically, "
                           "-f fast (execute quickly), "
                           "-h help,"
                           "--iface network interface\n");
                    return 1;
                    break;
                case 'q':
                    verbose = 0;
                    break;
                case 'f':
                    period = 1;
                    break;
                case 't':
                    terminate = 1;
                    break;
                case '-':
                    if (strcmp(argv[i], "--iface") == 0 && argc > i + 1)
                    {
                        i++;
                        iface = argv[i];
                        j = 1;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }

    signal(SIGINT, ctrlc);

    std::ostream &out = verbose ? std::cout : null_out;

    Device dev("testcpp");
    // if (iface)
    //     dev.graph().set_iface(iface);
    // out << "Created Device with interface " << dev.graph().iface() << std::endl;

    // make a copy of the device to check reference counting
    Device devcopy(dev);

    // Signal sig = dev.add_signal(Direction::INCOMING, "in1", 1, Type::INT32, "meters")
    //                  .set_callback(standard_handler);
    // dev.remove_signal(sig);
    // Signal insig1 = dev.add_signal(Direction::INCOMING, "in2", 1, Type::INT32).set_callback(instance_handler_in);
    Signal insig2 = dev.add_signal(Direction::INCOMING, "in3", 2, Type::INT32).set_callback(instance_handler_in);
    // dev.add_signal(Direction::INCOMING, "in4", 2, Type::INT32).set_callback(simple_handler);

    Signal outsig1 = dev.add_signal(Direction::OUTGOING, "out1", 1, Type::INT32, "na").set_callback(instance_handler_out);
    // dev.remove_signal(sig);
    Signal outsig2 = dev.add_signal(Direction::OUTGOING, "out2", 2, Type::INT32, "meters").set_callback(instance_handler_out);

    out << "waiting" << std::endl;
    while (!dev.ready() && !done)
    {
        dev.poll(10);
    }
    out << "ready" << std::endl;

    Graph graph;
    Map map(dev.signals(Direction::OUTGOING)[0], dev.signals(Direction::INCOMING)[0]);
    map.push();

    while (!map.ready() && !done)
    {
        dev.poll(10);
    }
    dev.start();

    std::vector<double> v1(1);
    std::vector<double> v2(2);

    // i = 0;
    while (val < 7000 && !done)
    {
        // v1[0] = val;
        v2[0] = val;
        v2[1] = val + 1;
        // out << v[0] << std::endl;
        // Signal s1 = *dev.signals().filter(Property::NAME, "in2", Operator::EQUAL);
        // Signal s2 = *dev.signals().filter(Property::NAME, "in3", Operator::EQUAL);

        // insig1.set_value(v[0]);
        // insig2.set_value(v[0] + 1);
        // outsig1.set_value(v1[0]);
        outsig2.set_value(v2);
        graph.poll(period);
    }
    dev.stop();
    // thread_object1.join();

    thread_object2.join();
    return result;
}
