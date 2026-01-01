#include <zephyr/types.h>
#include <zephyr/kernel.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <soc.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

#include <bluetooth/services/nus.h>

#include <stdio.h>
#include <string.h>

#include "bluetooth.h"

// Bluetooth
#define STACKSIZE CONFIG_BT_NUS_THREAD_STACK_SIZE
#define PRIORITY 7

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN	(sizeof(DEVICE_NAME) - 1)

static struct bt_conn *current_conn;
static struct k_work adv_work;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_VAL),
};

static char message[64];
static unsigned int message_length;
static unsigned int message_position;
static bool message_received;
static char pending_message[64];

static void adv_work_handler(struct k_work *work)
{
	int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));

	if (err) {
		return;
	}
}

static void advertising_start(void)
{
	k_work_submit(&adv_work);
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	current_conn = bt_conn_ref(conn);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	if (current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}
}

static void recycled_cb(void)
{
	advertising_start();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected        = connected,
	.disconnected     = disconnected,
	.recycled         = recycled_cb,
};

static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data,
			  uint16_t len)
{
	for (uint16_t pos = 0; pos != len;) {
		// /* Keep the last byte of TX buffer for potential LF char. */
		size_t tx_data_size = sizeof(message) - 1;

		if ((len - pos) > tx_data_size) {
			message_length = tx_data_size;
		} else {
			message_length = (len - pos);
		}

		memcpy(&message[message_position], &data[pos], len);

		pos += message_length;
		message_position += message_length;

		// /* Append the LF character when the CR character triggered
		//  * transmission from the peer.
		//  */
		if ((pos == len) && (data[len - 1] == '#')) {
			memcpy(&pending_message[0], &message[0], message_position);
			message_received = true;
			message_length = 0;
			message_position = 0;
		}
	}
}

static struct bt_nus_cb nus_cb = {
	.received = bt_receive_cb,
};

void error(void)
{
	while (true) {
		/* Spin for ever */
		k_sleep(K_MSEC(1000));
	}
}

void bluetooth_init(void)
{
	int err = 0;

	err = bt_enable(NULL);
	if (err) {
		error();
	}

	err = bt_nus_init(&nus_cb);
	if (err) {
		error();
	}

	k_work_init(&adv_work, adv_work_handler);
	advertising_start();
}

char * bluetooth_get_message(void)
{
    return &pending_message[0];
}

bool bluetooth_message_received(void)
{
    bool value = message_received;

    if (value)
    {
        message_received = false;
    }

    return value;
}
