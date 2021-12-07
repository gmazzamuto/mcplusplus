FROM python:3.10-slim-bullseye as builder

RUN apt-get update && apt-get install -y --no-install-recommends cmake make g++ libboost-thread-dev libboost-system-dev libhdf5-dev swig

COPY . /home

RUN set -ex \
	\
    && mkdir /build && cd /build \
    && cmake -DPYTHON_INCLUDE_DIR=$(python -c "import sysconfig; print(sysconfig.get_paths()['include'])") -DPYTHON_LIBRARY=$(python -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))") -DCMAKE_INSTALL_PREFIX=/local /home \
    && make -j4 install


FROM python:3.10-slim-bullseye
RUN apt-get update && apt-get install -y --no-install-recommends libboost-thread1.74.0 libboost-system1.74.0 libhdf5-cpp-103
COPY --from=builder /local /local
ENV PYTHONPATH=/local/lib/ LD_LIBRARY_PATH=/local/lib/