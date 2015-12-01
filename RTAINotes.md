# Introduction #

The page is meant to keep track of ideas, links, notes, etc. regarding RTAI and its setup.


# Details #
The Linux Journal has an overview ([1](http://www.linuxjournal.com/article/232), [2](http://www.linuxjournal.com/article/5742), [3](http://www.linuxjournal.com/article/5904)) of Linux real-time systems and a comparison of real-time packages.


RTAI Compatible Hardware:
  * May need to use Redhat Linux to install meIII software, because of the RPM files.
  * RTAI/Linux supports the Measurement Instrument cards in the lab: PCI-DDA08/12 and PCI-DAS1602/12.  You can find the full list of supported measurement hardware, including NI cards, [here](http://www.comedi.org/hardware.html).
  * Interesting [thread](http://forums.ni.com/ni/board/message?board.id=280&thread.id=967) on NI forum on issues to consider when adding hardware to RTAI.

Plausible Redhat Linux install [instructions](https://www.rtai.org/RTAILAB/RTAI-TARGET-HOWTO.txt) from rtai.org.

The Linux Boot process and info on initrd from IBM: ([1](http://www.ibm.com/developerworks/linux/library/l-linuxboot/), [2](http://www.ibm.com/developerworks/linux/library/l-initrd.html))

The different Linux [real-time approaches](http://www.ibm.com/developerworks/linux/library/l-real-time-linux/)

It appears that our new rtos will be the [pre-emptible kernel](http://rt.wiki.kernel.org/index.php/Main_Page) at kernel.org.