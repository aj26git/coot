
#include "c-beta-deviations.hh"

#include "geometry/residue-and-atom-specs.hh"

// return a map or a list at some stage
void
coot::get_c_beta_deviations(mmdb::Manager *mol) {

   int imod = 1;
   mmdb::Model *model_p = mol->GetModel(imod);
   if (model_p) {
      int n_chains = model_p->GetNumberOfChains();
      for (int ichain=0; ichain<n_chains; ichain++) {
	 std::cout << "ichain " << ichain << std::endl;
	 mmdb::Chain *chain_p = model_p->GetChain(ichain);
	 int nres = chain_p->GetNumberOfResidues();
	 for (int ires=0; ires<nres; ires++) {
	    mmdb::Residue *residue_p = chain_p->GetResidue(ires);
	    int n_atoms = residue_p->GetNumberOfAtoms();
	    std::map<std::string, c_beta_deviation_t> cbdm = get_c_beta_deviations(residue_p);
	    if (cbdm.size()) {
	       std::map<std::string, c_beta_deviation_t>::const_iterator it;
	       it = cbdm.begin();
	       std::cout << " " << residue_spec_t(residue_p) << " " << it->second.dist << std::endl;
	    }
	 }
      }
   }

}

std::map<std::string, coot::c_beta_deviation_t>
coot::get_c_beta_deviations(mmdb::Residue *residue_p) {

   std::map<std::string, c_beta_deviation_t> m;

   int n_atoms = residue_p->GetNumberOfAtoms();
   mmdb::Atom *at_0 = nullptr;
   mmdb::Atom *at_1 = nullptr;
   mmdb::Atom *at_2 = nullptr;
   mmdb::Atom *at_3 = nullptr;
   std::map<std::string, atom_quad> alt_conf_map;
   for (int iat=0; iat<n_atoms; iat++) {
      mmdb::Atom *at = residue_p->GetAtom(iat);
      std::string atom_name(at->GetAtomName());
      std::string alt_conf(at->altLoc);
      if (atom_name == " N  ") alt_conf_map[alt_conf].atom_1 = at;
      if (atom_name == " CA ") alt_conf_map[alt_conf].atom_2 = at;
      if (atom_name == " C  ") alt_conf_map[alt_conf].atom_3 = at;
      if (atom_name == " CB ") alt_conf_map[alt_conf].atom_4 = at;
   }

   // this doesn't work when CB has alt-conf but N (or CA) does not.
   // needs reworking... construct a quad for all CBs in residue, the
   // atoms of which fall back to '' if needed.

   std::map<std::string, atom_quad> ::const_iterator it;
   for(it=alt_conf_map.begin(); it!=alt_conf_map.end(); it++) {
      const atom_quad &q = it->second;
      if (q.filled_p()) {
	 clipper::Coord_orth CB_real_pos(q.atom_4->x, q.atom_4->y, q.atom_4->z); // no coot-utils.h included
	 clipper::Coord_orth CB_ideal_pos = make_CB_ideal_pos(q);
	 double dsqrd = (CB_ideal_pos-CB_real_pos).lengthsq();
	 double d = sqrt(dsqrd);
	 c_beta_deviation_t cbd(q.atom_4, CB_ideal_pos, d);
	 const std::string &key = it->first;
	 m[key] = cbd;
      }
   }

   return m;
}

clipper::Coord_orth
coot::make_CB_ideal_pos(const coot::atom_quad &q) {

   clipper::Coord_orth pt_1(q.atom_1->x, q.atom_1->y, q.atom_1->z);
   clipper::Coord_orth pt_2(q.atom_2->x, q.atom_2->y, q.atom_2->z);
   clipper::Coord_orth pt_3(q.atom_3->x, q.atom_3->y, q.atom_3->z);

   double l = 1.53;
   double a1 = clipper::Util::d2rad(111.0);
   double a2 = clipper::Util::d2rad(111.7);
   double t1 = clipper::Util::d2rad(124.4);
   double t2 = clipper::Util::d2rad(123.9);
   clipper::Coord_orth pt_trial1 = clipper::Coord_orth(pt_1, pt_3, pt_2, l, a1, t1);
   clipper::Coord_orth pt_trial2 = clipper::Coord_orth(pt_1, pt_3, pt_2, l, a1, t2);

   clipper::Coord_orth pt_trial = 0.5 * (pt_trial1 + pt_trial2);

   return pt_trial;

}
