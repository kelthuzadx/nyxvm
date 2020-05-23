FROM gcc
RUN apt-get update
RUN apt-get install -y cmake python3
WORKDIR /root
COPY . /root/
RUN cd build && cmake ..
RUN cd build && make -j8
RUN cd build && chmod +x nyx
# test
RUN python runtest.py

