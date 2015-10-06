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
	struct device platform = {0};
	struct kunit_host *hc0 = NULL, *hc1 = NULL;
	struct mmc_host *host0 = NULL, *host1 = NULL;
	struct mmc_card *card0 = NULL, *card1 = NULL;
	struct dentry *dev_tree;
	int ret = -1;
	char buf[PAGE_SIZE] = {0};
	char expected[] = ".\n" \
			  "|\n" \
			  "+-- platform\n" \
			  "    |\n" \
			  "    +-- kunit-hc.0\n" \
			  "    |   |\n" \
			  "    |   +-- mmc0 [host]\n" \
			  "    |       |\n" \
			  "    |       +-- mmc0:0001 [card]\n" \
			  "    |           |\n" \
			  "    |           +-- mmcblk0\n" \
			  "    |               |\n" \
			  "    |               +-- mmcblk0p1\n" \
			  "    |               |\n" \
			  "    |               +-- mmcblk0p2\n" \
			  "    |               |\n" \
			  "    |               +-- mmcblk0p3\n" \
			  "    |               |\n" \
			  "    |               +-- mmcblk0p4\n" \
			  "    |               |\n" \
			  "    |               +-- mmcblk0p5\n" \
			  "    |               |\n" \
			  "    |               +-- mmcblk0p6\n" \
			  "    |               |\n" \
			  "    |               +-- mmcblk0p7\n" \
			  "    |               |\n" \
			  "    |               +-- mmcblk0p8\n" \
			  "    |\n" \
			  "    +-- kunit-hc.1\n" \
			  "        |\n" \
			  "        +-- mmc1\n" \
			  "            |\n" \
			  "            +-- mmc1:0001\n" \
			  "                |\n" \
			  "                +-- mmcblk1\n" \
			  "                    |\n" \
			  "                    +-- mmcblk1p1\n" \
			  "                    |\n" \
			  "                    +-- mmcblk1p2\n" \
			  "                    |\n" \
			  "                    +-- mmcblk1p3\n" \
			  "                    |\n" \
			  "                    +-- mmcblk1p4\n";

	if (!kut_dev_init(&platform, NULL, "platform"))
		return -1;

	if (kut_mmc_init(&platform, &hc0, &host0, &card0, 8))
		goto exit;

	if (kut_mmc_init(&platform, &hc1, &host1, &card1, 4))
		goto exit;

	if (!mmc_create_dev_tree_debugfs(card0, NULL))
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
	kut_mmc_uninit(hc1, host1, card1);
	kut_mmc_uninit(hc0, host0, card0);
	kut_dev_uninit(&platform);

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

