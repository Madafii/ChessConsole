FROM debian:stable-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake wget git ca-certificates && rm -rf /var/lib/apt/lists/*
WORKDIR /web-static
# Do later doesn't work like this because can't build so either build separate project in web or something
COPY CMakeLists.txt main.cpp /web-static/
COPY chess/ /web-static/chess/
COPY web/ /web-static/web/

RUN ls -la /web-static && ls -la /web-static/chess && ls -la /web-static/web

# If you use a single header for cpp-httplib, ensure it's in the project.
# RUN mkdir -p build && cd build && cmake .. && cmake --build . --config Release
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release

# Runtime stage
FROM debian:stable-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 ca-certificates && rm -rf /var/lib/apt/lists/*

WORKDIR /web-static
COPY --from=build /web-static/build/web/chess-web /web-static
COPY --from=build /web-static/web/chess-web /web-static

EXPOSE 8080
ENV PORT=8080
CMD ["./chess-web"]
