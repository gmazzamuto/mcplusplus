FROM ubuntu:bionic as builder

RUN apt-get update && apt-get install -y --no-install-recommends cmake make g++ libboost-thread-dev libboost-system-dev libhdf5-dev swig python3-dev

COPY . /home

RUN set -ex \
	\
    && mkdir /build && cd /build \
    && cmake -DPYTHON_INCLUDE_DIR=$(python3 -c "import sysconfig; print(sysconfig.get_paths()['include'])") -DPYTHON_LIBRARY=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))") -DCMAKE_INSTALL_PREFIX=/local /home \
    && make -j4 install


FROM ubuntu:bionic
RUN apt-get update && apt-get install -y --no-install-recommends libboost-thread1.65.1 libboost-system1.65.1 libhdf5-cpp-100 python3
COPY --from=builder /local /local
ENV PYTHONPATH=/local/lib/ LD_LIBRARY_PATH=/local/lib/