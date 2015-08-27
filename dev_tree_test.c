#include <linux/device.h>
#include <linux/debugfs.h>
#include <linux/mmc/dev_tree.h>

#include <linux/kut_device.h>
#include <linux/mmc/kut_bus.h>
#include <linux/mmc/kut_host.h>
#include <linux/kut_namei.h>

#include <unit_test.h>

static int mmc_dev_tree(void)
{
	struct mmc_host *host = NULL;
	struct mmc_card *card = NULL;
	struct dentry *dev_tree;
	int ret = -1;
	char buf[PAGE_SIZE] = {0};
	char expected[] = ".\n" \
			  "|\n" \
			  "+-- mmc0 [host]\n" \
			  "    |\n" \
			  "    +-- mmc0:0001 [card]\n" \
			  "        |\n" \
			  "        +-- mmcblk0\n";

	if (kut_mmc_init(NULL, &host, &card, 0))
		return -1;

	if (!mmc_create_dev_tree_debugfs(card, NULL))
		goto exit;

	dev_tree = kut_dentry_lookup(NULL, "dev_tree");
	if (!dev_tree)
		goto exit;

	kut_dentry_read(dev_tree, buf, sizeof(buf));
	printf("%s\n", buf);
	if (strcmp(buf, expected))
		goto exit;

	ret = 0;
exit:
	debugfs_remove_recursive(&kern_root);
	kut_mmc_uninit(NULL, host, card);
	return ret;
}
 
static struct single_test dev_tree_tests[] = {
	{
		description: "get the dev_tree for some mmc device hierarchy",
		func: mmc_dev_tree,
	},
};

struct unit_test ut_dev_tree = {
	.module = "dev_tree",
	.description = "Device Tree Hierarchy",
	.tests = dev_tree_tests,
	.count = ARRAY_SZ(dev_tree_tests),
};

