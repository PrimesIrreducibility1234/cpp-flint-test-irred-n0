FROM ubuntu:24.04

WORKDIR /app

RUN apt-get update && apt-get install -y \
    g++ \
    python3 \
    python3-pip \
    libflint-dev \
    libgmp-dev \
    libmpfr-dev \
    && rm -rf /var/lib/apt/lists/*

COPY main.cpp /app/main.cpp
COPY temp.cpp /app/temp.cpp
COPY app.py /app/app.py
COPY requirements.txt /app/requirements.txt

RUN pip3 install --break-system-packages -r requirements.txt

RUN g++ main.cpp -std=c++17 -O2 -Wall -Wextra -lflint -lgmp -lmpfr -o main
RUN g++ temp.cpp -std=c++17 -O2 -Wall -Wextra -lflint -lgmp -lmpfr -o temp

EXPOSE 10000

CMD ["gunicorn", "-b", "0.0.0.0:10000", "app:app"]
