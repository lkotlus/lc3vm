CC = clang
CFLAGS = -Isrc -Wall -Wextra -std=c11

SRC_DIR = src
BUILD_DIR = build
TARGET = lc3vm

SRCS := $(shell find src -name '*.c')
OBJS := $(patsubst src/%.c, build/%.o, $(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

format:
	clang-format -i src/**/*.c src/**/*.h
