/*
Milu v3.0 - A Higher Order Mutation Testing Tool

Copyright (c) 2015, Fan Wu
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the University College London nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Mutator.h"
#include "ASTNodeUtil.h"

static gboolean mutator_milu_replace_calloc_with_alloca_node_checking(ASTNode *);
static gboolean mutator_milu_replace_calloc_with_alloca_clean(ASTNode * node, gint type);
static gboolean mutator_milu_replace_calloc_with_alloca_mutate(ASTNode * node, gint type);

static ASTNode * tmpNode;
static gint tmpIndex;

Mutator * mutator_milu_replace_calloc_with_alloca()
{
	Mutator * mut = mutator_new("(Memory)Replace calloc() with alloca()", "");
	mut->node_checking = & mutator_milu_replace_calloc_with_alloca_node_checking;
	mut->mutate = & mutator_milu_replace_calloc_with_alloca_mutate;
	mut->clean = & mutator_milu_replace_calloc_with_alloca_clean;
	mut->size = 1;
	return mut;
}


static gboolean mutator_milu_replace_calloc_with_alloca_node_checking(ASTNode * node)
{
	return has_ASTNode_calloc_call(node);
}

static gboolean mutator_milu_replace_calloc_with_alloca_mutate(ASTNode * node, gint type)
{
	ASTNode *left, *right, *replace;
	ASTNode * child=node->children;
	gint index=1;
	while(child){
		if(is_ASTNode_calloc_call(child)) break;
		child=child->next_sibling;
		index++;
	}
	if(!child){
		return FALSE;
	}
	switch(type)
	{
		case 1:
			left=child->children->children->next_sibling;
			right=left->next_sibling;
			replace=ASTNode_new_alloca_substitution_for_calloc(left->children, right->children);
			left->children=NULL;
			right->children=NULL;
			tmpIndex=index;
			tmpNode=child;
			replace_subtree_with(child, replace);
			return TRUE;

		default:
			break;
	}

	return FALSE;
}

static gboolean mutator_milu_replace_calloc_with_alloca_clean(ASTNode * node, gint type)
{
	node=ASTNode_get_nth_child(node, tmpIndex);
	ASTNode *left=tmpNode->children->children->next_sibling;
	ASTNode *right=left->next_sibling;
	ASTNode *left_expr=node->children->children->next_sibling;
	ASTNode *right_expr=left_expr->children->next_sibling->children->children;
	left_expr=left_expr->children->children->children;
	left->children=left_expr;
	right->children=right_expr;
	left_expr->parent=left;
	right_expr->parent=right;
	replace_subtree_with(node, tmpNode);
	tmpNode=NULL;
	tmpIndex=0;
	return TRUE;
}
