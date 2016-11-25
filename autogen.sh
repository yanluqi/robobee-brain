#!/bin/sh

if [ `uname -s` = Darwin ] ; then
# libtoolize is glibtoolize on OSX
  LIBTOOLIZE=glibtoolize
else
  LIBTOOLIZE=libtoolize
fi

echo adding AUTHORS NEWS README ChangeLog files when not existing &&
touch AUTHORS NEWS README ChangeLog &&
echo aclocal &&
aclocal &&
echo $LIBTOOLIZE --copy --automake &&
$LIBTOOLIZE --copy --automake &&
echo autoheader &&
autoheader &&
echo autoconf &&
autoconf &&
echo automake --copy --add-missing &&
automake --copy --add-missing &&
echo Now run build shell script. It will automatically configure and compile the program.
