#include <linux/device.h>
#include <linux/stat.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

struct device_iterator {
	struct device *dev;
	struct device *dev_card;
	struct device *dev_host;
	char *buf;
	int consumed;
	int depth;
};

static int dev_hierarchy_show_rec(struct device *dev, void *data)
{
	struct device_iterator *di = (struct device_iterator *)data;
	char *extra;
	int i, ret;

	if (!dev->p)
		return 0;

	if (dev == di->dev_card)
		extra = " [card]";
	else if (dev == di->dev_host)
		extra = " [host]";
	else
		extra = "";

	for (i = 0; i < 2; i++) {
		int indent;

		for (indent = 0; indent < di->depth; indent++) {
			di->consumed += snprintf(di->buf + di->consumed,
				PAGE_SIZE - di->consumed, "    ");
		}
		di->consumed += snprintf(di->buf + di->consumed,
			PAGE_SIZE - di->consumed, i ? "+-- %s%s\n" : "|\n",
			dev_name(dev), extra);
	}

	di->depth++;
	ret = device_for_each_child(dev, data, dev_hierarchy_show_rec);
	di->depth--;
	return ret;
}

static int mmc_dev_to_card_host(struct device *dev, struct mmc_card **card,
	struct mmc_host **host)
{
	struct mmc_card *c;
	struct mmc_host *h;

	c = mmc_dev_to_card(dev);
	if (!c) {
		pr_err("%s(): no card\n", __func__);
		return -1;
	}

	h = c->host;
	if (!h) {
		pr_err("%s(): no host\n", __func__);
		return -1;
	}

	*card = c;
	*host = h;
	return 0;
}

static ssize_t dev_hierarchy_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mmc_card *card;
	struct mmc_host *host;
	struct device_iterator di = {0};

	if (mmc_dev_to_card_host(dev, &card, &host))
		return 0;

	di.dev_card = dev;
	di.dev_host = &host->class_dev;
	di.buf = buf;
	di.consumed = 0;
	di.depth = 0;

	while (dev->parent)
		dev = dev->parent; /* rewind */

	di.consumed += snprintf(di.buf + di.consumed, PAGE_SIZE - di.consumed,
		".\n");

	dev_hierarchy_show_rec(dev, &di);
	return di.consumed;
}

DEVICE_ATTR(dev_hierarchy, S_IRUGO, dev_hierarchy_show, NULL);

