
if [ $# != 3 ] ; then
   echo must provide gtk-version and 2 file arguments
   echo we got: $*
   exit 2
fi

pre="$2"
post="$3"
gtk2="$1"

echo "#ifdef USE_PYTHON " > "$post"
if [ "$gtk2" = gtk2 ] ; then
   echo "#ifdef COOT_USE_GTK2_INTERFACE"  >> $post
else 
   echo "#ifndef COOT_USE_GTK2_INTERFACE"  >> $post
fi    
# cat "$pre" >> "$post" 
sed -e 's/char .doc = (((PyCFunctionObject .)obj) -> m_ml -> ml_doc);/const char *doc = (((PyCFunctionObject *)obj) -> m_ml -> ml_doc);/' \
    -e 's/char .c = methods[i].ml_doc/const char *c = methods[i].ml_doc/' \
    -e 's/char .name = c + 10/const char *name = c + 10/' \
  "$pre" >> "$post" 
echo "#endif // COOT_USE_GTK2_INTERFACE" >> $post
echo "#endif // USE_PYTHON " >> "$post"
