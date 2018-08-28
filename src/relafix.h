#ifndef _RELAFIX_H
#define _RELAFIX_H

#include <stdbool.h>

typedef struct rf_Set rf_Set;
typedef struct rf_SetElement rf_SetElement;
typedef struct rf_Relation rf_Relation;
typedef struct rf_Error rf_Error;

char * rf_set_to_string(rf_Set *);
char * rf_relation_to_string(rf_Relation *);

// DEPRECATED
rf_Set *        rf_set_new(int n, rf_SetElement **elements);
rf_Set *        rf_set_clone(const rf_Set *set);

rf_Set *        rf_set_new_intersection(const rf_Set *, const rf_Set *);
rf_Set *        rf_set_new_powerset(const rf_Set *set);

bool            rf_set_equal(const rf_Set *a, const rf_Set *b);
bool            rf_set_is_subset(const rf_Set *subset, const rf_Set *superset);

int             rf_set_get_cardinality(const rf_Set *set);
bool            rf_set_contains_element(const rf_Set *set, const rf_SetElement *element);
int             rf_set_get_element_index(const rf_Set *set, const rf_SetElement *element);

void            rf_set_free(rf_Set *set);

rf_SetElement * rf_set_element_new_string(char *value);
rf_SetElement * rf_set_element_new_set(rf_Set *value);
rf_SetElement * rf_set_element_clone(const rf_SetElement *element);

bool            rf_set_element_equal(const rf_SetElement *a, const rf_SetElement *b);

void            rf_set_element_free(rf_SetElement *element);

// --

rf_Relation *   rf_relation_new(rf_Set *domain1, rf_Set *domain2, bool *table);

rf_Relation *   rf_relation_new_union(rf_Relation *relation_1, rf_Relation *relation_2, rf_Error *error);
rf_Relation *   rf_relation_new_intersection(rf_Relation *relation_1, rf_Relation *relation_2, rf_Error *error);
rf_Relation *   rf_relation_new_complement(rf_Relation *relation, rf_Error *error);
rf_Relation *   rf_relation_new_concatenation(rf_Relation *relation_1, rf_Relation *relation_2, rf_Error *error);
rf_Relation *   rf_relation_new_converse(const rf_Relation *relation, rf_Error *error);

bool            rf_relation_is_homogeneous(const rf_Relation *relation);

bool            rf_relation_is_antisymmetric(const rf_Relation *relation);
bool            rf_relation_is_asymmetric(const rf_Relation *relation);
bool            rf_relation_is_difunctional(const rf_Relation *relation);
bool            rf_relation_is_equivalent(const rf_Relation *relation);
bool            rf_relation_is_irreflexive(const rf_Relation *relation);
bool            rf_relation_is_partial_order(const rf_Relation *relation);
bool            rf_relation_is_preorder(const rf_Relation *relation);
bool            rf_relation_is_reflexive(const rf_Relation *relation);
bool            rf_relation_is_symmetric(const rf_Relation *relation);
bool            rf_relation_is_transitive(const rf_Relation *relation);
bool            rf_relation_is_lattice(const rf_Relation *relation, rf_Error *error);
bool            rf_relation_is_sublattice(rf_Relation *superlattice, rf_Relation *sublattice, rf_Error *error);
bool            rf_relation_is_lefttotal(const rf_Relation *relation);
bool            rf_relation_is_functional(const rf_Relation *relation);
bool            rf_relation_is_function(const rf_Relation *relation);
bool            rf_relation_is_surjective(const rf_Relation *relation);
bool            rf_relation_is_injective(const rf_Relation *relation);
bool            rf_relation_is_bijective(const rf_Relation *relation);

#endif
