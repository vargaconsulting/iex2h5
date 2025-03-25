#
#   ALL RIGHTS RESERVED.
#   _________________________________________________________________________________
#   NOTICE: All information contained  herein is, and remains the property  of  Varga
#   Consulting and  its suppliers, if  any. The intellectual and  technical  concepts
#   contained herein are proprietary to Varga Consulting and its suppliers and may be
#   covered  by  Canadian and  Foreign Patents, patents in process, and are protected
#   by  trade secret or copyright law. Dissemination of this information or reproduc-
#   tion  of  this  material is strictly forbidden unless prior written permission is
#   obtained from Varga Consulting.
#
#   Copyright © <2017-2025> Varga Consulting, Toronto, On     info@vargaconsulting.ca
#   _________________________________________________________________________________
#

DIRS =  src/iex2h5 

BUILDDIRS = $(DIRS:%=build-%)
INSTALLDIRS = $(DIRS:%=install-%)
CLEANDIRS = $(DIRS:%=clean-%)
TESTDIRS = $(DIRS:%=test-%)

all: $(BUILDDIRS)
$(DIRS): $(BUILDDIRS)
$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

install: $(INSTALLDIRS) all
$(INSTALLDIRS):
	$(MAKE) -C $(@:install-%=%) install

test: $(TESTDIRS) all lib
$(TESTDIRS):  
	$(MAKE) -C $(@:test-%=%) test


clean: $(CLEANDIRS)
$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean

.PHONY: subdirs $(DIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(INSTALLDIRS)
.PHONY: subdirs $(TESTDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: all install clean  




