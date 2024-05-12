BUILD_DIR := ./build
SRC_DIR := ./src
TEST_DIR := ./test
INC_DIRS := $(shell find $(SRC_DIR) -type d)
TEST_INC := $(shell find $(TEST_DIR) -type d)


TARO_LIB := $(BUILD_DIR)/taro.a
TARO_TEST := $(BUILD_DIR)/taro_test

TARO_SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')
TARO_OBJS := $(TARO_SRCS:%=$(BUILD_DIR)/%.o)
TARO_DEPS := $(TARO_OBJS:%.o=%.d)

TEST_SRCS := $(shell find $(TEST_DIR) -type f -name '*.c') $(TARO_SRCS)
TEST_OBJS := $(TEST_SRCS:%=$(BUILD_DIR)/%.o) $(TARO_OBJS)
TEST_DEPS := $(TEST_OBJS:%.o=%.d) $(TARO_DEPS)

INC_FLAGS := $(INC_DIRS:%=-I%)
CFLAGS := $(CFLAGS) -Wall -Werror -MMD -MP

$(TARO_TEST): $(TARO_LIB) $(TEST_OBJS)
	$(CC) -o $@ $(CFLAGS) $(INC_FLAGS) $(LDFLAGS) $^

$(TARO_LIB): $(TARO_OBJS)
	ar rcs $@ $^

$(BUILD_DIR)/%.c.o: %.c
	-mkdir -p $(dir $@)
	$(CC) -c -o $@ $(CFLAGS) $(INC_FLAGS) $(LDFLAGS) $^ 

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: test
test: $(TARO_TEST)
	$(TARO_TEST)

-include $(TARO_DEPS) $(TEST_DEPS)
