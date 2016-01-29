#!/bin/bash
cd src
qmake ./prak_plugins.pro
make -j4
qmake ./prak_gui_plugins.pro
make -j4
mv prak_plugins ../bin
mv prak_gui_plugins ../bin
cd plugins/MedianFilter
qmake
make -j4
cd ../BoxFilter
qmake
make -j4
cd ../
mv libBoxFilter.so ../../bin/plugins
mv libMedianFilter.so ../../bin/plugins
cd ../..

