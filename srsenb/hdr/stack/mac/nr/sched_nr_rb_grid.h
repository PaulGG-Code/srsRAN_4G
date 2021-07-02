/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_SCHED_NR_RB_GRID_H
#define SRSRAN_SCHED_NR_RB_GRID_H

#include "../sched_common.h"
#include "lib/include/srsran/adt/circular_array.h"
#include "sched_nr_interface.h"
#include "sched_nr_pdcch.h"
#include "sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

using pdsch_bitmap = srsran::bounded_bitset<25, true>;
using pusch_bitmap = srsran::bounded_bitset<25, true>;

using pdsch_t      = sched_nr_interface::pdsch_grant;
using pdsch_list_t = sched_nr_interface::pdsch_list_t;

using pusch_list = sched_nr_interface::pusch_list_t;

const static size_t MAX_CORESET_PER_BWP = 3;
using slot_coreset_list                 = srsran::bounded_vector<coreset_region, MAX_CORESET_PER_BWP>;

struct bwp_slot_grid {
  uint32_t          bwp_id;
  uint32_t          slot_idx;
  bool              is_dl, is_ul;
  pdsch_bitmap      dl_rbgs;
  pusch_bitmap      ul_rbgs;
  pdsch_list_t      pdschs;
  pusch_list_t      puschs;
  slot_coreset_list coresets;
  pucch_list_t      pucchs;

  bwp_slot_grid() = default;
  explicit bwp_slot_grid(const sched_cell_params& cell_params, uint32_t bwp_id_, uint32_t slot_idx_);
  void reset();
};

struct bwp_res_grid {
  bwp_res_grid(const sched_cell_params& cell_cfg_, uint32_t bwp_id_);

  bwp_slot_grid&           operator[](tti_point tti) { return slots[tti.sf_idx()]; };
  const bwp_slot_grid&     operator[](tti_point tti) const { return slots[tti.sf_idx()]; };
  uint32_t                 id() const { return bwp_id; }
  uint32_t                 nof_prbs() const { return cell_cfg->cell_cfg.nof_prb; }
  const sched_cell_params& cell_params() const { return *cell_cfg; }
  const bwp_cfg_t&         bwp_cfg() const { return cell_cfg->cell_cfg.bwps[id() - 1]; }

private:
  uint32_t                 bwp_id;
  const sched_cell_params* cell_cfg = nullptr;

  srsran::bounded_vector<bwp_slot_grid, TTIMOD_SZ> slots;
};

struct cell_res_grid {
  const sched_cell_params*                                        cell_cfg = nullptr;
  srsran::bounded_vector<bwp_res_grid, SCHED_NR_MAX_BWP_PER_CELL> bwps;

  explicit cell_res_grid(const sched_cell_params& cell_cfg);
};

class slot_bwp_sched
{
public:
  explicit slot_bwp_sched(uint32_t bwp_id, cell_res_grid& phy_grid_);

  alloc_result alloc_pdsch(slot_ue& ue, const rbgmask_t& dl_mask);
  alloc_result alloc_pusch(slot_ue& ue, const rbgmask_t& dl_mask);

  const sched_cell_params& cfg;

private:
  srslog::basic_logger& logger;
  bwp_res_grid&         bwp_grid;

  tti_point tti_rx;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_RB_GRID_H
