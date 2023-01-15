
#include "linux/cpumask.h"
#include "linux/gfp_types.h"
#include <linux/of_address.h>
#include <linux/bits.h>
#include <linux/irqreturn.h>
#include <linux/mod_devicetable.h>
#include <linux/stddef.h>
#include <linux/iopoll.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/math.h>
#include <linux/compiler.h>
#include <linux/compiler_attributes.h>
#include <linux/dev_printk.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/gfp.h>
#include <linux/kern_levels.h>
#include <linux/list.h>
#include <linux/of.h>
#include <linux/printk.h>
#include <linux/property.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/dmaengine.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_dma.h>
#include <linux/clk.h>
#include <linux/io-64-nonatomic-lo-hi.h>

MODULE_LICENSE("GPL v2");

//
// fill the descriptor
static void fill_desc(struct dma_async_tx_descriptor *desc, dma_addr_t src,
		      dma_addr_t dst, unsigned int length, unsigned int num)
{
	// 硬件注册
}
// 设备
// static void device_probe(struct platform_device *pdev)
// {
// 	struct dma_device *dev =
// 		devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
// 	dma_cap_mask_t mask;
// 	dma_cap_zero(mask);
// 	dma_cap_set(DMA_SLAVE | DMA_PRIVATE, mask);
// }
//分配dma slave通道
static struct dma_chan *alloc_dma_slave_chan(struct device *dev,
					     const char *name)
{
	struct dma_chan *chan = dma_request_chan(dev, name);
	return chan;
}
//配置DMA channel的参数
static int set_dma_channel_config(struct dma_chan *chan, struct device *dev,
				  struct dma_slave_config *config)
{
	// struct dma_slave_config *config =
	// 	devm_kzalloc(dev, sizeof(*config), GFP_KERNEL);
	config->direction = DMA_MEM_TO_DEV;
	//config->dst_addr = phy_addr + txdr_offset;
	config->dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
	config->dst_maxburst = 1;
	config->src_maxburst = 128;
	config->dst_maxburst = 128;

	int ret = dmaengine_slave_config(chan, config);
	return ret;
}
// 获取传输描述符
static struct dma_async_tx_descriptor *
get_desc_for_trans(struct dma_chan *chan, struct scatterlist *sgl,
		   unsigned int sg_len, enum dma_transfer_direction direction,
		   unsigned long flags, void *context,
		   struct dma_slave_config *config)
{
	struct scatterlist *sg;
	dma_addr_t src = 0, dst = 0, addr = 0, slave_addr = 0;
	unsigned int length = 0, num = 0;
	unsigned int i;

	if (sgl == NULL) {
		// error("sgl is null!\n");
		return NULL;
	}
	// 分配内存
	struct dma_async_tx_descriptor *desc =
		kzalloc(sizeof(*desc), GFP_NOWAIT);
	if (!desc)
		return NULL;
	dma_async_tx_descriptor_init(desc, chan);

	for_each_sg(sgl, sg, sg_len, i) {
		addr = sg_dma_address(sg);
		length = sg_dma_len(sg);
		if (direction == DMA_MEM_TO_DEV) {
			slave_addr = config->dst_addr;
			src = addr;
			dst = slave_addr;
		} else if (direction == DMA_DEV_TO_MEM) {
			slave_addr = config->src_addr;
			src = slave_addr;
			dst = addr;
		}
		fill_desc(desc, src, dst, length, num);
		num++;
	}
	return desc;
}