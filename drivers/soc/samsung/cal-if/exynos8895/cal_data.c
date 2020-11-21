#include "../pmucal_common.h"
#include "../pmucal_cpu.h"
#include "../pmucal_local.h"
#include "../pmucal_rae.h"
#include "../pmucal_system.h"

#include "pmucal_cpu_exynos8895.h"
#include "pmucal_local_exynos8895.h"
#include "pmucal_p2vmap_exynos8895.h"
#include "pmucal_system_exynos8895.h"

#include "cmucal-node.c"
#include "cmucal-qch.c"
#include "cmucal-sfr.c"
#include "cmucal-vclk.c"
#include "cmucal-vclklut.c"

#include "clkout_exynos8895.c"
#include "acpm_dvfs_exynos8895.h"
#include "asv_exynos8895.h"

void (*cal_data_init)(void) = NULL;

#if defined(CONFIG_SEC_FACTORY) || defined(CONFIG_SEC_DEBUG)
enum ids_info {
	tg,
	lg,
	bg,
	g3dg,
	mifg,
	bids,
	gids,
};

int asv_ids_information(enum ids_info id) {
	int res;

	switch (id) {
		case tg:
			res = asv_get_table_ver();
			break;
		case lg:
			res = asv_get_grp(dvfs_cpucl1);
			break;
		case bg:
			res = asv_get_grp(dvfs_cpucl0);
			break;
		case g3dg:
			res = asv_get_grp(dvfs_g3d);
			break;
		case mifg:
			res = asv_get_grp(dvfs_mif);
			break;
		case bids:
			res = asv_get_ids_info(dvfs_cpucl0);
			break;
		case gids:
			res = asv_get_ids_info(dvfs_g3d);
			break;
		default:
			res = 0;
			break;
		};
	return res;
}
#endif

#if defined(CONFIG_SEC_PM_DEBUG)
enum dvfs_id {
	cal_asv_dvfs_big,
	cal_asv_dvfs_little,
	cal_asv_dvfs_g3d,
	cal_asv_dvfs_mif,
	cal_asv_dvfs_int,
	cal_asv_dvfs_cam,
	cal_asv_dvfs_disp,
	cal_asv_dvs_g3dm,
	num_of_dvfs,
};

enum asv_group {
	asv_max_lv,
	dvfs_freq,
	dvfs_voltage,
	dvfs_group,
	table_group,
	num_of_asc,
};

int asv_get_information(enum dvfs_id id, enum asv_group grp, unsigned int lv) {

	int max_lv = 0, volt = 0, group = 0;

	switch (id) {
		case cal_asv_dvfs_big:
			max_lv = 0;
			volt = 0;
			group = asv_get_grp(dvfs_cpucl0);
			break;
		case cal_asv_dvfs_little:
			max_lv = 0;
			volt = 0;
			group = asv_get_grp(dvfs_cpucl1);
			break;
		case cal_asv_dvfs_g3d:
			max_lv = 0;
			volt = 0;
			group = asv_get_grp(dvfs_g3d);
			break;
		case cal_asv_dvfs_mif:
			max_lv = 0;
			volt = 0;
			group = asv_get_grp(dvfs_mif);
			break;
		case cal_asv_dvfs_int:
			max_lv = 0;
			volt = 0;
			group = asv_get_grp(dvfs_int);
			break;
		case cal_asv_dvfs_cam:
			max_lv = 0;
			volt = 0;
			group = asv_get_grp(dvfs_cam);
			break;
		case cal_asv_dvfs_disp:
			max_lv = 0;
			volt = 0;
			group = asv_get_grp(dvfs_disp);
			break;
		case cal_asv_dvs_g3dm:
			max_lv = 0;
			volt = 0;
			group = 0;
			break;
		default:
			break;
	}

	if (grp == asv_max_lv)
		return max_lv;
	else if (grp == dvfs_voltage)
		return volt;
	else if (grp == dvfs_group)
		return group;
	else
		return 0;
}
#endif /* CONFIG_SEC_PM_DEBUG */

#include <linux/debugfs.h>

static int asv_summary_show(struct seq_file *s, void *d)
{

	seq_printf(s, "Table ver: %d\n", asv_get_table_ver());
	seq_printf(s, "CL0: %d\n", asv_tbl->mngs_asv_group);
	seq_printf(s, "CL1: %d\n", asv_tbl->apollo_asv_group);
	seq_printf(s, "G3D: %d\n", asv_tbl->g3d_asv_group);
	seq_printf(s, "MIF: %d\n", asv_tbl->mif_asv_group);
	seq_printf(s, "INT: %d\n", asv_tbl->int_asv_group);
	seq_printf(s, "CAM: %d\n", asv_tbl->cam_disp_asv_group);
	seq_printf(s, "CP: %d\n", asv_tbl->cp_asv_group);

	seq_printf(s, "IDS (b,g): %d, %d\n",
			asv_get_ids_info(dvfs_cpucl0),
			asv_get_ids_info(dvfs_g3d));

	return 0;
}

static int asv_summary_open(struct inode *inode, struct file *file)
{
	return single_open(file, asv_summary_show, inode->i_private);
}

const static struct file_operations asv_summary_fops = {
	.open		= asv_summary_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init cal_data_late_init(void)
{
	debugfs_create_file("asv_summary", 0444, NULL, NULL, &asv_summary_fops);

	return 0;
}

late_initcall(cal_data_late_init);

