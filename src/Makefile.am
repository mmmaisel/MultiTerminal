bin_PROGRAMS=MultiTerminal

AM_CFLAGS=
AM_CXXFLAGS=

if DEBUG
    AM_CFLAGS += -g3 -O0 -D_DEBUG
    AM_CXXFLAGS += -g3 -O0 -D_DEBUG
else
    AM_CFLAGS += -O2
    AM_CXXFLAGS += -O2
endif

MultiTerminal_datarootdir=$(prefix)/share
MultiTerminal_icondir=$(datarootdir)/MultiTerminal

AM_CFLAGS += -DICON_DIR=\"$(MultiTerminal_icondir)/\"
AM_CXXFLAGS += -DICON_DIR=\"$(MultiTerminal_icondir)/\"

MultiTerminal_CFLAGS = \
	$(AM_CFLAGS) $(WX_CFLAGS_ONLY) $(YAMLCPP_CFLAGS) $(PTHREAD_CFLAGS)
MultiTerminal_CPPFLAGS = \
	$(AM_CFLAGS) $(WX_CPPFLAGS) $(YAMLCPP_CFLAGS) $(PTHREAD_CFLAGS) $(BOOST_CPPFLAGS)
MultiTerminal_CXXFLAGS = \
	$(AM_CXXFLAGS) $(WX_CXXFLAGS_ONLY) $(YAMLCPP_CXXFLAGS) $(PTHREAD_CXXFLAGS) $(BOOST_CXXFLAGS)
MultiTerminal_LDADD= \
	$(WX_LIBS) $(YAMLCPP_LIBS) $(PTHREAD_LIBS) $(PTHREAD_CFLAGS) $(BOOST_LDFLAGS)

# Experimental precompiled file creation
# If this does not work, remove these stdafx.h.gch lines
BUILT_SOURCES = stdafx.h.gch
stdafx.h.gch: Makefile
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(WX_CXXFLAGS) $(YAMLCPP_CXXFLAGS) $(PTHREAD_CXXFLAGS) \
	-c stdafx.h -o stdafx.h.gch
CLEANFILES = stdafx.h.gch

MultiTerminal_SOURCES= \
	main.cpp \
	itoa.cpp \
	cmdHistory.cpp \
	connection.cpp \
	gui/aboutDlg.cpp \
	gui/mainframe.cpp \
	gui/backend.cpp \
	gui/multiBaseInput.cpp \
	gui/multiBaseOutput.cpp \
	gui/termFileDlgExtra.cpp \
	gui/termInstance.cpp \
	resource/messages.cpp \
	resource/icons.cpp \
	resource/configFile.cpp
	
install-data-local:
	$(MKDIR_P) "$(DESTDIR)$(MultiTerminal_icondir)"
	cp ../icons/add-preset.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/clear-rx.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/clear-term.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/clear-tx.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/connect.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/del-preset.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/disconnect.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/edit-preset.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/iconLicenses.txt "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/pin-active.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/pin-inactive.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/save-term.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/start-log.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/stop-log.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/tx-file.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
	cp ../icons/MultiTerminal_128.png "$(DESTDIR)$(MultiTerminal_icondir)/icons/"
