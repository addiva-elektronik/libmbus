# ------------------------------------------------------------------------------
#  Copyright (C) 2012, Robert Johansson <rob@raditex.nu>, Raditex Control AB
#  All rights reserved.
# 
#  rSCADA 
#  http://www.rSCADA.se
#  info@raditex.nu
# 
# ------------------------------------------------------------------------------

if [ ! -f Makefile ]; then
	#
	# regenerate automake files
	#
    echo "Running autotools..."
    ./autogen.sh && ./configure
fi

debuild -i -us -uc -b 
#sudo pbuilder build $(NAME)_$(VERSION)-1.dsc
