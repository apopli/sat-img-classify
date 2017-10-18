CC = g++

LDLIBJPEG=-ljpeg
LDLIBPNG=-lpng
LDLIBTIFF=-ltiff

LIBS = $(LDLIBJPEG) $(LDLIBPNG) $(LDLIBTIFF)

SRCS1 = img_traindata.c imageio.c
SRCS2 = svm.cpp train_cv.c
SRCS3 = svm.cpp sat_train.c
SRCS4 = svm.cpp sat_classify.c
SRCS5 = svm.cpp imageio.c test_classify.c

ifneq ($(LDLIBJPEG),)
	CJPEG=-DUSE_LIBJPEG
endif
ifneq ($(LDLIBPNG),)
	CPNG=-DUSE_LIBPNG
endif
ifneq ($(LDLIBTIFF),)
	CTIFF=-DUSE_LIBTIFF
endif

CFLAGS=$(CJPEG) $(CPNG) $(CTIFF)

OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
OBJS3 = $(SRCS3:.c=.o)
OBJS4 = $(SRCS4:.c=.o)
OBJS5 = $(SRCS5:.c=.o)

.PHONY: clean rebuild	
	
IMG2TRAIN : $(OBJS1)
	$(CC) -o IMG2TRAIN $(OBJS1) $(LIBS)
CROSSVAL : $(OBJS2)
	$(CC) -o CROSSVAL $(OBJS2)
TRAIN : $(OBJS3)
	$(CC) -o TRAIN $(OBJS3)
CLASSIFY_TRAIN : $(OBJS4)
	$(CC) -o CLASSIFY_TRAIN $(OBJS4)
CLASSIFY_TEST : $(OBJS5)
	$(CC) -o CLASSIFY_TEST $(OBJS5) $(LIBS)
        
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ IMG2TRAIN CROSSVAL TRAIN CLASSIFY_TRAIN CLASSIFY_TEST classified.jpg testdata testdata.classified 

rebuild: clean CLASSIFY_TEST
