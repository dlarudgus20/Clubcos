// Copyright (c) 2014, 임경현 (dlarudgus20)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
* @file rbtree.c
* @date 2016. 2. 17
* @author dlarudgus20
* @copyright The BSD (2-Clause) License
*/

#include "rbtree.h"
#include "assert.h"

enum { RB_RED, RB_BLK };

static void csInsertionBalancing(RbTree *pTree, RbTreeNode *pNode);
static void csErasionBalancing(RbTree *pTree, RbTreeNode *pNode);

static RbTreeNode *csGetMaxNode(RbTreeNode *root, uint32_t *pDepth);
static RbTreeNode *csGetMinNode(RbTreeNode *root, uint32_t *pDepth);

static void csRotateLeft(RbTreeNode *pNode);
static void csRotateRight(RbTreeNode *pNode);

static inline bool csIsBlkOrNil(RbTreeNode *pNode)
{
	return (pNode == NULL || pNode->color == RB_BLK);
}
static inline bool csIsRed(RbTreeNode *pNode)
{
	return (pNode != NULL && pNode->color == RB_RED);
}

static inline RbTreeNode *csGetSibling(RbTreeNode *pNode)
{
	RbTreeNode *parent = pNode->parent;
	if (parent == NULL)
		return NULL;

	return (pNode == parent->left) ? parent->right : parent->left;
}

void ckRbTreeInit(RbTree *pTree)
{
	pTree->root = NULL;
}

RbTreeNode *ckRbTreeInsert(RbTree *pTree, RbTreeNode *pNode)
{
	if (pTree->root == NULL)
	{
		pNode->left = pNode->right = pNode->parent = NULL;
		pNode->color = RB_BLK;
		pTree->root = pNode;
		return pNode;
	}
	else
	{
		RbTreeNode *parent = ckRbTreeFind(pTree, pNode->key);

		if (parent->key == pNode->key)
		{
			return parent;
		}
		else
		{
			pNode->parent = parent;
			pNode->left = pNode->right = NULL;

			if (parent->key < pNode->key)
				parent->right = pNode;
			else
				parent->left = pNode;

			csInsertionBalancing(pTree, pNode);

			return pNode;
		}
	}
}

static void csInsertionBalancing(RbTree *pTree, RbTreeNode *pNode)
{
	RbTreeNode *parent = pNode->parent;
	RbTreeNode *grand;
	RbTreeNode *uncle;

	// stage 1
	if (parent == NULL)
	{
		pNode->color = RB_BLK;
		return;
	}

	// stage 2
	if (parent->color == RB_BLK)
	{
		pNode->color = RB_RED;
		return;
	}

	grand = parent->parent;
	if (grand != NULL)
		uncle = (grand->left == parent) ? grand->right : grand->left;
	else
		uncle = NULL;

	// stage 3
	if (uncle != NULL && uncle->color == RB_RED)
	{
		pNode->color = RB_RED;
		parent->color = RB_BLK;
		uncle->color = RB_BLK;
		grand->color = RB_RED;
		
		csInsertionBalancing(pTree, grand);
	}
	else
	{
		pNode->color = RB_RED;

		// stage 4
		bool bRequireReconfig = false;

		if (pNode == parent->right && parent == grand->left)
		{
			csRotateLeft(parent);
			bRequireReconfig = true;
		}
		else if (pNode == parent->left && parent == grand->right)
		{
			csRotateRight(parent);
			bRequireReconfig = true;
		}

		if (bRequireReconfig)
		{
			pNode = parent;
			parent = pNode->parent;
			grand = parent->parent;
		}

		// stage 5
		parent->color = RB_BLK;
		grand->color = RB_RED;
		if (pNode == parent->left)
			csRotateRight(grand);
		else
			csRotateLeft(grand);

		if (pTree->root == grand)
		{
			assert(parent->parent == NULL);
			pTree->root = parent;
		}
	}
}

void ckRbTreeErase(RbTree *pTree, RbTreeNode *pNode)
{
	RbTreeNode *parent = pNode->parent;

	if (pNode->left != NULL && pNode->right != NULL)
	{
		RbTreeNode *successor;

		RbTreeNode *l_s, *r_s;
		uint32_t l_depth, r_depth;

		l_s = csGetMaxNode(pNode->left, &l_depth);
		r_s = csGetMinNode(pNode->right, &r_depth);
		successor = (l_depth > r_depth) ? l_s : r_s;

		ckRbTreeErase(pTree, successor);

		if (parent != NULL)
		{
			if (parent->left == pNode)
				parent->left = successor;
			else
				parent->right = successor;
		}
		successor->parent = pNode->parent;
		successor->left = pNode->left;
		successor->right = pNode->right;

		if (successor->left != NULL)
			successor->left->parent = successor;

		if (successor->right != NULL)
			successor->right->parent = successor;

		if (pTree->root == pNode)
			pTree->root = successor;
	}
	else
	{
		RbTreeNode *child = (pNode->left != NULL) ? pNode->left : pNode->right;
		RbTreeNode *sibling = NULL;

		if (parent != NULL)
		{
			if (parent->left == pNode)
			{
				parent->left = child;
				sibling = parent->right;
			}
			else
			{
				parent->right = child;
				sibling = parent->left;
			}
		}

		//assert((parent != NULL) -> (sibling != NULL));
		assert(parent == NULL || sibling != NULL);

		if (child != NULL)
			child->parent = pNode->parent;

		// case 0
		if (pNode->color == RB_BLK)
		{
			if (child != NULL)
				child->color = RB_BLK;
			else
				csErasionBalancing(pTree, sibling);
		}
	}
}

static void csErasionBalancing(RbTree *pTree, RbTreeNode *sibling)
{
	RbTreeNode *parent;
	RbTreeNode *pNode;
	bool NodeIsLeft;

	// case 1 (if root)
	if (sibling == NULL)
		return;

	parent = sibling->parent;
	if (sibling == parent->left)
	{
		pNode = parent->right;
		NodeIsLeft = false;
	}
	else
	{
		pNode = parent->left;
		NodeIsLeft = true;
	}
	assert(pNode == NULL || pNode->color == RB_BLK);

	// case 2
	if (sibling->color == RB_RED)
	{
		if (pTree->root == parent)
			pTree->root = sibling;

		parent->color = RB_RED;
		sibling->color = RB_BLK;
		if (NodeIsLeft)
		{
			sibling = sibling->left;
			csRotateLeft(parent);
		}
		else
		{
			sibling = sibling->right;
			csRotateRight(parent);
		}
		assert(sibling != NULL);
	}

	// case 3
	if (parent->color == RB_BLK && sibling->color == RB_BLK &&
		csIsBlkOrNil(sibling->left) && csIsBlkOrNil(sibling->right))
	{
		sibling->color = RB_RED;
		csErasionBalancing(pTree, csGetSibling(parent));
	}
	// case 4
	else if (parent->color == RB_RED && csIsBlkOrNil(sibling->left) && csIsBlkOrNil(sibling->right))
	{
		parent->color = RB_BLK;
		sibling->color = RB_RED;
	}
	else
	{
		// case 5
		if (NodeIsLeft && csIsRed(sibling->left) && csIsBlkOrNil(sibling->right))
		{
			sibling->color = RB_RED;
			sibling->left->color = RB_BLK;

			sibling = sibling->left;
			csRotateRight(sibling);
		}
		else if (!NodeIsLeft && csIsRed(sibling->right) && csIsBlkOrNil(sibling->left))
		{
			sibling->color = RB_RED;
			sibling->right->color = RB_BLK;

			sibling = sibling->right;
			csRotateLeft(sibling);
		}

		// case 6
		assert(sibling->color == RB_BLK);
		assert((NodeIsLeft && csIsRed(sibling->right)) || (!NodeIsLeft && csIsRed(sibling->left)));

		sibling->color = parent->color;
		parent->color = RB_BLK;

		if (pTree->root == parent)
			pTree->root = sibling;

		if (NodeIsLeft)
		{
			sibling->right->color = RB_BLK;
			csRotateLeft(parent);
		}
		else
		{
			sibling->left->color = RB_BLK;
			csRotateRight(parent);
		}
	}
}

RbTreeNode *ckRbTreeFind(RbTree *pTree, uint32_t key)
{
	RbTreeNode *pNode = pTree->root;
	RbTreeNode *pNext;

	while (1)
	{
		if (pNode->key < key)
			pNext = pNode->right;
		else if (pNode->key > key)
			pNext = pNode->left;
		else
			return pNode;

		if (pNext == NULL)
			return pNode;
		else
			pNode = pNext;
	}
}

RbTreeNode *ckRbTreeFirst(RbTree *pTree)
{
	uint32_t depth;
	return csGetMinNode(pTree->root, &depth);
}

RbTreeNode *ckRbTreeNext(RbTreeNode *pNode)
{
	if (pNode->right != NULL)
	{
		uint32_t depth;
		return csGetMinNode(pNode->right, &depth);
	}

	while (1)
	{
		RbTreeNode *parent = pNode->parent;

		if (parent == NULL)
		{
			return NULL;
		}
		else if (parent->left == pNode)
		{
			return parent;
		}
		else
		{
			pNode = parent;
		}
	}
}

static RbTreeNode *csGetMaxNode(RbTreeNode *root, uint32_t *pDepth)
{
	RbTreeNode *pNode = root;

	*pDepth = 1;
	while (1)
	{
		if (pNode->right == NULL)
			return pNode;

		pNode = pNode->right;
		(*pDepth)++;
	}
}

static RbTreeNode *csGetMinNode(RbTreeNode *root, uint32_t *pDepth)
{
	RbTreeNode *pNode = root;

	*pDepth = 1;
	while (1)
	{
		if (pNode->left == NULL)
			return pNode;

		pNode = pNode->left;
		(*pDepth)++;
	}
}

static void csRotateLeft(RbTreeNode *pNode)
{
	RbTreeNode *parent = pNode->parent;
	RbTreeNode *right = pNode->right;

	assert(right != NULL);

	if (right->left != NULL)
		right->left->parent = pNode;

	pNode->right = right->left;
	pNode->parent = right;

	right->left = pNode;
	right->parent = parent;

	if (parent != NULL)
	{
		if (parent->left == pNode)
			parent->left = right;
		else
			parent->right = right;
	}
}

static void csRotateRight(RbTreeNode *pNode)
{
	RbTreeNode *parent = pNode->parent;
	RbTreeNode *left = pNode->left;

	assert(left != NULL);

	if (left->right != NULL)
		left->right->parent = pNode;

	pNode->left = left->right;
	pNode->parent = left;

	left->right = pNode;
	left->parent = parent;

	if (parent != NULL)
	{
		if (parent->left == pNode)
			parent->left = left;
		else
			parent->right = left;
	}
}
