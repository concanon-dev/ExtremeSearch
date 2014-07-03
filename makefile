# Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
# Makefile for Extreme Search
#
OSTYPE := $(shell uname)
PROCESSOR := $(shell uname -m)
VERSION := `echo ${version} | sed -e 's/_/./g'`

platform.Cygwin := Win/x64
platform.CYGWIN_NT-6.1 := Win/x64
platform.darwin := Mac
platform.Darwin := Mac
platform.linux  := Linux/x64
platform.Linux  := Linux/x64
platform.GNU/Linux  := Linux/x64
PLATFORM := ${platform.${OSTYPE}}

releaseplatform.Cygwin := Windows
releaseplatform.CYGWIN_NT-6.1 := Windows
releaseplatform.darwin := Darwin
releaseplatform.Darwin := Darwin
releaseplatform.linux  := Linux
releaseplatform.Linux  := Linux
releaseplatform.GNU/Linux  := Linux
RELEASEPLATFORM := ${releaseplatform.${OSTYPE}}

objdir.Cygwin := obj/Win/x64
objdir.CYGWIN_NT-6.1 := obj/Win/x64
objdir.darwin := obj/Mac
objdir.Darwin := obj/Mac
objdir.linux  := obj/Linux/x64
objdir.Linux  := obj/Linux/x64
objdir.GNU/Linux  := obj/Linux/x64
OBJDIR := ${objdir.${OSTYPE}}

bindir.Cygwin := bin/Win/x64
bindir.CYGWIN_NT-6.1 := bin/Win/x64
bindir.darwin := bin/Mac
bindir.Darwin := bin/Mac
bindir.linux  := bin/Linux/x64
bindir.Linux  := bin/Linux/x64
bindir.GNU/Linux  := bin/Linux/x64
BINDIR := ${bindir.${OSTYPE}}


RELEASEDIR := release/xtreme


compresscmd.Cygwin := cd release; tar -cvzf ess_win_${version}_s.tgz xtreme/*
compresscmd.CYGWIN_NT-6.1 := cd release; tar -cvzf ess_win_${version}_s.tgz xtreme/*
compresscmd.darwin := cd release; ln -s 64bit xtreme/bin/Darwin/32bit; tar -cvzf ess_mac_${version}_s.tgz xtreme/*
compresscmd.Darwin := cd release; ln -s 64bit xtreme/bin/Darwin/32bit; tar -cvzf ess_mac_${version}_s.tgz xtreme/*
compresscmd.linux  := cd release; tar -cvzf ess_linux_${version}_s.tgz xtreme/*
compresscmd.Linux  := cd release; tar -cvzf ess_linux_${version}_s.tgz xtreme/*
compresscmd.GNU/Linux  := cd release; tar -cvzf ess_linux_${version}_s.tgz xtreme/*
COMPRESSCMD := ${compresscmd.${OSTYPE}}

#gcc.Cygwin := x86_64-w64-mingw32-gcc -D _UNICODE
gcc.Cygwin := $(PROCESSOR)-w64-mingw32-gcc -D _UNICODE
gcc.CYGWIN_NT-6.1 := $(PROCESSOR)-w64-mingw32-gcc -D _UNICODE
gcc.darwin :=gcc -g -fgnu89-inline -no-cpp-precomp
gcc.Darwin :=gcc -g -fgnu89-inline -no-cpp-precomp
gcc.linux := gcc -g
gcc.Linux := gcc -g
gcc.GNU/Linux := gcc -g
GCC := ${gcc.${OSTYPE}}


licenselib.Cygwin :=  -lws2_32 
licenselib.CYGWIN_NT-6.1 :=  -lws2_32 
licenselib.darwin := -Wl,-rpath,@executable_path/. 
licenselib.Darwin := -Wl,-rpath,@executable_path/. 
licenselib.linux := -Wl,-R,$(ORIGIN) 
licenselib.Linux := -Wl,-R,$(ORIGIN)
licenselib.GNU/Linux := -Wl,-R,$(ORIGIN) 
LICENSELIB := ${licenselib.${OSTYPE}}


winobjs.Cygwin := obj/${PLATFORM}/strsep.o
winobjs.CYGWIN_NT-6.1 := obj/${PLATFORM}/strsep.o
winobjs.darwin := 
winobjs.Darwin := 
winobjs.linux := 
winobjs.Linux := 
winobjs.GNU/Linux := 
WINOBJS := ${winobjs.${OSTYPE}}


CFLAGS=-Ofast -Wall
LDFLAGS=-lm -g -dead_strip
SOURCES= fix-intersplunk.c saAutoRegression.c saContext.c saContextCreate.c saCSV.c saGeoLiteCity.c saHash.c saHedge.c saInsertUniqueValue.c saLinearCorrelation.c saLinearRegression.c saListDir.c saMatrixArgs.c saParser.c saQuadRegression.c saSignal.c saConcept.c saSplunk.c saSpearmanCorrelation.c saStatistics.c xsAggregateAutoRegression.c xsAggregateCorrelation.c xsAggregateLinearRegression.c xsAggregateQuadRegression.c xsAggregateSpearmanCorrelation.c xsApplyAutoRegression.c xsApplyAutoRegressionFromFile.c xsApplyLinearRegression.c xsApplyLinearRegressionFromFile.c xsApplyQuadRegression.c xsApplyQuadRegressionFromFile.c xsCloneContext.c xsCloneConcept.c xsCreateContext.c xsCreateConcept.c xsDeleteContext.c xsDeleteConcept.c xsDiscoverTrend.c xsDisplayContext.c xsDisplayConcept.c xsDisplayWhere.c xsFindBestConcept.c xsFindMembership.c xsGetCompatibility.c xsGetDistance.c xsListContexts.c xsListConcepts.c xsListUOM.c xsMergeContexts.c xsPerformAutoRegression.c xsPerformCorrelation.c xsPerformLinearRegression.c xsPerformQuadRegression.c xsPerformSpearmanCorrelation.c xsRenameContext.c csvtest.c xsWhere.c xspreautoregress.c xsprecorrelate.c xsPredict.c xsprequadregress.c xspreregress.c xsprespearmancorrelate.c xsrepredict.c strsep.c saTokenize.tab.o lex.yy.o

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES = xsAggregateAutoRegression xsAggregateCorrelation xsAggregateLinearRegression xsAggregateQuadRegression xsAggregateSpearmanCorrelation xsApplyAutoRegression xsApplyAutoRegressionFromFile xsApplyLinearRegression xsApplyLinearRegressionFromFile xsApplyQuadRegression xsApplyQuadRegressionFromFile xsCloneContext xsCloneConcept xsCreateContext xsCreateConcept xsDeleteContext xsDeleteConcept xsDiscoverTrend xsDisplayContext xsDisplayConcept xsDisplayWhere xsFindBestConcept xsFindMembership xsGetCompatibility xsGetDistance xsGetWhereCix xsListContexts xsListConcepts xsListUOM xsMergeContexts xsPerformAutoRegression xsPerformCorrelation xsPerformLinearRegression xsPerformQuadRegression xsPerformSpearmanCorrelation xsRenameContext xsWhere xspreautoregress xsprecorrelate xsPredict xsprequadregress xspreregress xsprespearmancorrelate xsrepredict



all: $(EXECUTABLES)

#
# Executables
#
xsAggregateAutoRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsAggregateAutoRegression.o $(OBJDIR)/saAutoRegression.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsAggregateCorrelation: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsAggregateCorrelation.o $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o   $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsAggregateLinearRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsAggregateLinearRegression.o $(OBJDIR)/saLinearRegression.o $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsAggregateQuadRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsAggregateQuadRegression.o $(OBJDIR)/saQuadRegression.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsAggregateSpearmanCorrelation: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsAggregateSpearmanCorrelation.o $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o  $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsApplyAutoRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsApplyAutoRegression.o $(OBJDIR)/saCSV.o $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsApplyAutoRegressionFromFile: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsApplyAutoRegressionFromFile.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsApplyLinearRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsApplyLinearRegression.o $(OBJDIR)/saCSV.o $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsApplyLinearRegressionFromFile: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsApplyLinearRegressionFromFile.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsApplyQuadRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsApplyQuadRegression.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsApplyQuadRegressionFromFile: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsApplyQuadRegressionFromFile.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsCloneContext: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsCloneContext.o $(OBJDIR)/saContext.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsCloneConcept: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsCloneConcept.o $(OBJDIR)/saContext.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsCreateContext: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsCreateContext.o $(OBJDIR)/saContext.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsCreateConcept: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsCreateConcept.o $(OBJDIR)/saContext.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsDeleteContext: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsDeleteContext.o $(OBJDIR)/saContext.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsDeleteConcept: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsDeleteConcept.o $(OBJDIR)/saContext.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsDiscoverTrend: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsDiscoverTrend.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(WINOBJS) $(LDFLAGS)
xsDisplayContext: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsDisplayContext.o  $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsDisplayConcept: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsDisplayConcept.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(WINOBJS) $(LDFLAGS)
xsDisplayWhere: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsDisplayWhere.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saParser.o $(OBJDIR)/saHash.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(OBJDIR)/saTokenize.tab.o $(OBJDIR)/lex.yy.o $(LICENSELIB) $(LDFLAGS)
xsFindBestConcept: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsFindBestConcept.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saHash.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsFindMembership: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsFindMembership.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsGetCompatibility: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsGetCompatibility.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saAutoRegression.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB)  $(LDFLAGS)
xsGetDistance: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsGetDistance.o $(OBJDIR)/saGeoLiteCity.o $(OBJDIR)/saStatistics.o $(OBJDIR)/saContext.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saHash.o $(OBJDIR)/saCSV.o $(OBJDIR)/csvtest.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(WINOBJS) $(LDFLAGS)
xsGetWhereCix: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsWhere.o $(OBJDIR)/csvtest.o  $(OBJDIR)/saParser.o $(OBJDIR)/saHash.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(OBJDIR)/saTokenize.tab.o $(OBJDIR)/lex.yy.o $(LICENSELIB) $(LDFLAGS)
xsListContexts: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsListContexts.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saListDir.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsListConcepts: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsListConcepts.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(WINOBJS) $(LDFLAGS)
xsListUOM: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsListUOM.o $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(WINOBJS) $(LDFLAGS)
xsMergeContexts: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsMergeContexts.o  $(OBJDIR)/saContext.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsPerformAutoRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsPerformAutoRegression.o $(OBJDIR)/saAutoRegression.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(LICENSELIB) $(LDFLAGS)
xsPerformCorrelation: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsPerformCorrelation.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o  $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(LICENSELIB) $(LDFLAGS)
xsPerformLinearRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsPerformLinearRegression.o $(OBJDIR)/saLinearRegression.o  $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(LICENSELIB) $(LDFLAGS)
xsPerformQuadRegression: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsPerformQuadRegression.o $(OBJDIR)/saQuadRegression.o  $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsPerformSpearmanCorrelation: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsPerformSpearmanCorrelation.o $(OBJDIR)/saCSV.o $(OBJDIR)/saContext.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saConcept.o $(OBJDIR)/saSignal.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o   $(LICENSELIB) $(LDFLAGS)
xsRenameContext: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsRenameContext.o $(OBJDIR)/saContext.o $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o  $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsWhere: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsWhere.o $(OBJDIR)/saParser.o  $(OBJDIR)/saConcept.o $(OBJDIR)/saContextCreate.o $(OBJDIR)/saContext.o  $(OBJDIR)/saHash.o $(OBJDIR)/saSplunk.o $(OBJDIR)/saHedge.o $(OBJDIR)/saCSV.o $(OBJDIR)/csvtest.o $(OBJDIR)/saSignal.o   $(OBJDIR)/saTokenize.tab.o $(OBJDIR)/lex.yy.o $(LICENSELIB) $(LDFLAGS)
xspreautoregress: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xspreautoregress.o $(OBJDIR)/saAutoRegression.o $(OBJDIR)/saInsertUniqueValue.o $(OBJDIR)/saCSV.o $(OBJDIR)/csvtest.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsprecorrelate: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsprecorrelate.o $(OBJDIR)/saLinearCorrelation.o $(OBJDIR)/saStatistics.o $(OBJDIR)/saInsertUniqueValue.o  $(OBJDIR)/saMatrixArgs.o $(OBJDIR)/saCSV.o $(OBJDIR)/csvtest.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsPredict: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsPredict.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsprequadregress: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsprequadregress.o $(OBJDIR)/saQuadRegression.o $(OBJDIR)/saInsertUniqueValue.o  $(OBJDIR)/saCSV.o $(OBJDIR)/csvtest.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xspreregress: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xspreregress.o $(OBJDIR)/saLinearRegression.o $(OBJDIR)/saInsertUniqueValue.o $(OBJDIR)/saCSV.o $(OBJDIR)/csvtest.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsprespearmancorrelate: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsprespearmancorrelate.o $(OBJDIR)/saSpearmanCorrelation.o $(OBJDIR)/saStatistics.o  $(OBJDIR)/saInsertUniqueValue.o $(OBJDIR)/saMatrixArgs.o $(OBJDIR)/saCSV.o $(OBJDIR)/csvtest.o  $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)
xsrepredict: $(OBJECTS)
	$(GCC) -o $(BINDIR)/$@ $(CFLAGS) $(OBJDIR)/xsrepredict.o $(OBJDIR)/saCSV.o $(OBJDIR)/saSignal.o  $(LICENSELIB) $(LDFLAGS)

#
# Individual C files
#
csvtest.o:
	$(GCC) -O3 -c csvtest.c -o $(OBJDIR)/$@
saTokenize.tab.o: saTokenize.tab.c saTokenize.tab.h lex.yy.c
	$(GCC) -c  saTokenize.tab.c -o $(OBJDIR)/saTokenize.tab.o
lex.yy.o: 
	$(GCC) -c  lex.yy.c -o $(OBJDIR)/lex.yy.o
fix-intersplunk.o:
	$(GCC) -c fix-intersplunk.c -o $(OBJDIR)/$@
saAutoRegression.o:
	$(GCC) -O3 -c saAutoRegression.c -o $(OBJDIR)/$@
saContext.o:
	$(GCC) -O3 -c saContext.c -o $(OBJDIR)/$@
saContextCreate.o:
	$(GCC) -O3 -c saContextCreate.c -o $(OBJDIR)/$@
saCSV.o:
	$(GCC) -O3 -c saCSV.c -o $(OBJDIR)/$@
saGeoLiteCity.o:
	$(GCC) -O3 -c saGeoLiteCity.c -o $(OBJDIR)/$@
saHash.o:
	$(GCC) -O3 -c saHash.c -o $(OBJDIR)/$@
saHedge.o:
	$(GCC) -O3 -c saHedge.c -o $(OBJDIR)/$@
saInsertUniqueValue.o:
	$(GCC) -O3 -c saInsertUniqueValue.c -o $(OBJDIR)/$@
saLinearCorrelation.o:
	$(GCC) -O3 -c saLinearCorrelation.c -o $(OBJDIR)/$@
saLinearRegression.o:
	$(GCC) -O3 -c saLinearRegression.c -o $(OBJDIR)/$@
saListDir.o:
	$(GCC) -O3 -c saListDir.c -o $(OBJDIR)/$@
saMatrixArgs.o:
	$(GCC) -O3 -c saMatrixArgs.c -o $(OBJDIR)/$@
saParser.o:
	$(GCC) -O3 -c saParser.c -o $(OBJDIR)/$@
saQuadRegression.o:
	$(GCC) -O3 -c saQuadRegression.c -o $(OBJDIR)/$@
saSignal.o:
	$(GCC) -c saSignal.c -o $(OBJDIR)/$@
saConcept.o:
	$(GCC) -c saConcept.c -o $(OBJDIR)/$@
saSplunk.o:
	$(GCC) -O3 -c saSplunk.c -o $(OBJDIR)/$@
saSpearmanCorrelation.o:
	$(GCC) -O3 -c saSpearmanCorrelation.c -o $(OBJDIR)/$@
saStatistics.o:
	$(GCC) -O3 -c saStatistics.c -o $(OBJDIR)/$@
xsAggregateAutoRegression.o:
	$(GCC) -O3 -c xsAggregateAutoRegression.c -o $(OBJDIR)/$@
xsAggregateCorrelation.o:
	$(GCC) -O3 -c xsAggregateCorrelation.c -o $(OBJDIR)/$@
xsAggregateLinearRegression.o:
	$(GCC) -O3 -c xsAggregateLinearRegression.c -o $(OBJDIR)/$@
xsAggregateQuadRegression.o:
	$(GCC) -O3 -c xsAggregateQuadRegression.c -o $(OBJDIR)/$@
xsAggregateSpearmanCorrelation.o:
	$(GCC) -O3 -c xsAggregateSpearmanCorrelation.c -o $(OBJDIR)/$@
xsApplyAutoRegression.o:
	$(GCC) -O3 -c xsApplyAutoRegression.c -o $(OBJDIR)/$@
xsApplyAutoRegressionFromFile.o:
	$(GCC) -O3 -c xsApplyAutoRegressionFromFile.c -o $(OBJDIR)/$@
xsApplyLinearRegression.o:
	$(GCC) -O3 -c xsApplyLinearRegression.c -o $(OBJDIR)/$@
xsApplyLinearRegressionFromFile.o:
	$(GCC) -O3 -c xsApplyLinearRegressionFromFile.c -o $(OBJDIR)/$@
xsApplyQuadRegression.o:
	$(GCC) -O3 -c xsApplyQuadRegression.c -o $(OBJDIR)/$@
xsApplyQuadRegressionFromFile.o:
	$(GCC) -O3 -c xsApplyQuadRegressionFromFile.c -o $(OBJDIR)/$@
xsCloneContext.o:
	$(GCC) -c xsCloneContext.c -o $(OBJDIR)/$@
xsCloneConcept.o:
	$(GCC) -c xsCloneConcept.c -o $(OBJDIR)/$@
xsCreateContext.o:
	$(GCC) -c xsCreateContext.c -o $(OBJDIR)/$@
xsCreateConcept.o:
	$(GCC) -c xsCreateConcept.c -o $(OBJDIR)/$@
xsDeleteContext.o:
	$(GCC) -c xsDeleteContext.c -o $(OBJDIR)/$@
xsDeleteConcept.o:
	$(GCC) -c xsDeleteConcept.c -o $(OBJDIR)/$@
xsDiscoverTrend.o:
	$(GCC) -O3 -c xsDiscoverTrend.c -o $(OBJDIR)/$@
xsDisplayContext.o:
	$(GCC) -c xsDisplayContext.c -o $(OBJDIR)/$@
xsDisplayConcept.o:
	$(GCC) -c xsDisplayConcept.c -o $(OBJDIR)/$@
xsDisplayWhere.o:
	$(GCC) -O3 -c xsDisplayWhere.c -o $(OBJDIR)/$@
xsFindBestConcept.o:
	$(GCC) -O3 -c xsFindBestConcept.c -o $(OBJDIR)/$@
xsFindMembership.o:
	$(GCC) -O3 -c xsFindMembership.c -o $(OBJDIR)/$@
xsGetCompatibility.o:
	$(GCC) -O3 -c xsGetCompatibility.c -o $(OBJDIR)/$@
xsGetDistance.o:
	$(GCC) -O3 -c xsGetDistance.c -o $(OBJDIR)/$@
xsListContexts.o:
	$(GCC) -c xsListContexts.c -o $(OBJDIR)/$@
xsListConcepts.o:
	$(GCC) -c xsListConcepts.c -o $(OBJDIR)/$@
xsListUOM.o:
	$(GCC) -c xsListUOM.c -o $(OBJDIR)/$@
xsMergeContexts.o:
	$(GCC) -c xsMergeContexts.c -o $(OBJDIR)/$@
xsPerformAutoRegression.o:
	$(GCC) -O3 -c xsPerformAutoRegression.c -o $(OBJDIR)/$@
xsPerformCorrelation.o:
	$(GCC) -O3 -c xsPerformCorrelation.c -o $(OBJDIR)/$@
xsPerformLinearRegression.o:
	$(GCC) -O3 -c xsPerformLinearRegression.c -o $(OBJDIR)/$@
xsPerformQuadRegression.o:
	$(GCC) -O3 -c xsPerformQuadRegression.c -o $(OBJDIR)/$@
xsPerformSpearmanCorrelation.o:
	$(GCC) -O3 -c xsPerformSpearmanCorrelation.c -o $(OBJDIR)/$@
xsRenameContext.o:
	$(GCC) -c xsRenameContext.c -o $(OBJDIR)/$@
xsWhere.o:
	$(GCC) -O3 -c xsWhere.c -o $(OBJDIR)/$@
xsWhere3.o:
	$(GCC) -O3 -c xsWhere3.c -o $(OBJDIR)/$@
xspreautoregress.o:
	$(GCC) -O3 -c xspreautoregress.c -o $(OBJDIR)/$@
xsprecorrelate.o:
	$(GCC) -O3 -c xsprecorrelate.c -o $(OBJDIR)/$@
xsPredict.o:
	$(GCC) -O3 -c xsPredict.c -o $(OBJDIR)/$@
xsprequadregress.o:
	$(GCC) -O3 -c xsprequadregress.c -o $(OBJDIR)/$@
xspreregress.o:
	$(GCC) -O3 -c xspreregress.c -o $(OBJDIR)/$@
xsprespearmancorrelate.o:
	$(GCC) -O3 -c xsprespearmancorrelate.c -o $(OBJDIR)/$@
xsrepredict.o:
	$(GCC) -O3 -c xsrepredict.c -o $(OBJDIR)/$@
# strsep only for cgwin
strsep.o:
	$(GCC) -c strsep.c -o $(OBJDIR)/$@

saTokenize.tab.c saTokenize.tab.h: saTokenize.y
	bison -d saTokenize.y

lex.yy.c: saTokenize.l saTokenize.tab.h
	flex saTokenize.l

xsrelease: all
	@if [ "$(version)" == "" ]; then \
		echo "Usage: make xsrelease version=x_y_z where x_y_z is the current version"; \
	else \
		echo "Make Extreme Search Release $(VERSION) for platform $(OSTYPE)"; \
		$(shell mkdir -p $(BINDIR)) \
		$(shell mkdir -p $(OBJDIR)) \
		$(shell rm -rf $(RELEASEDIR)) \
		$(shell mkdir -p $(RELEASEDIR)/appserver/static) \
		$(shell mkdir -p $(RELEASEDIR)/bin) \
		$(shell mkdir -p $(RELEASEDIR)/static) \
		$(shell mkdir -p $(RELEASEDIR)/bin/$(RELEASEPLATFORM)/64bit) \
		$(shell mkdir -p $(RELEASEDIR)/default/data/ui/nav) \
		$(shell mkdir -p $(RELEASEDIR)/default/data/ui/views) \
		$(shell mkdir -p $(RELEASEDIR)/lookups) \
		$(shell mkdir -p $(RELEASEDIR)/metadata) \
		$(shell mkdir -p $(RELEASEDIR)/contexts) \
		$(shell grep -v "^version=" default/app.conf > xxx) \
		$(shell echo "version=$(VERSION)" >> xxx) \
		$(shell mv xxx default/app.conf) \
		echo "Updated version to: " $(VERSION) ;\
		$(shell cp -r appserver/* $(RELEASEDIR)/appserver/) \
		$(shell cp -r static/* $(RELEASEDIR)/static/) \
		$(shell cp version.info $(RELEASEDIR)) \
		$(shell cp bin/$(PLATFORM)/* $(RELEASEDIR)/bin/$(RELEASEPLATFORM)/64bit/) \
		$(shell cp -r default/* $(RELEASEDIR)/default/) \
		$(shell cp -r lookups/* $(RELEASEDIR)/lookups/) \
		$(shell cp -r metadata/* $(RELEASEDIR)/metadata/) \
		$(shell cp python/* $(RELEASEDIR)/bin/) \
		$(shell cp contexts/* $(RELEASEDIR)/contexts) \
		$(shell cp pngfiles/ess_400_s.png release/xtreme/appserver/static/images/ess_400.png) \
		$(COMPRESSCMD) ; \
	fi

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(BINDIR)/*.o
	rm -rf $(RELEASEDIR)
	rm -f saTokenize.tab.c
	rm -f saTokenize.tab.h
	rm -f lex.yy.c
