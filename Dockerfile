# --- Стадия сборки ---
FROM drogonframework/drogon:latest AS builder
WORKDIR /app
COPY . .
RUN mkdir -p build && cd build && cmake .. && make -j$(nproc)

# --- Финальный образ ---
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl gettext-base \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копируем скомпилированное приложение и шаблон конфига
COPY --from=builder /app/build/todo_app ./todo_app
COPY config.json.template .
COPY entrypoint.sh .
RUN chmod +x entrypoint.sh

# Копируем ВСЕ библиотеки из стадии сборки (проверенный способ)
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/lib /usr/lib
COPY --from=builder /lib /lib

# Обновляем кэш линковщика
RUN ldconfig

EXPOSE 5000
HEALTHCHECK --interval=10s --timeout=5s --retries=3 \
    CMD curl -f http://localhost:5000/health || exit 1
ENTRYPOINT ["./entrypoint.sh"]
