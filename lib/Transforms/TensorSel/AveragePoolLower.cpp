//===- AveragePoolLower.cpp -------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <onnc/Transforms/TensorSel/Lower.h>
#include <onnc/Transforms/TensorSel/Standards/AveragePoolLower.h>
#include <onnc/IR/Compute/AveragePool.h>
#include "SetDefaultAttributes.h"
#include <onnc/IR/IRBuilder.h>

using namespace onnc;

//===----------------------------------------------------------------------===//
// AveragePoolLower
//===----------------------------------------------------------------------===//
AveragePoolLower::AveragePoolLower()
{
}

AveragePoolLower::~AveragePoolLower()
{
}

int AveragePoolLower::isMe(const xNode& pNode) const
{
  if (pNode.kind() == xSymbol("AveragePool"))
    return kStdLower;
  return kNotMe;
}

ComputeOperator*
AveragePoolLower::activate(ComputeGraph& pGraph, xNode& pNode) const
{
  // check input/output number
  if (pNode.inputs().size() != 1)
    return nullptr;

  if (pNode.outputs().size() != 1)
    return nullptr;

  // check input/output name
  for (xValue* xv : pNode.inputs()) {
    if (!xv->has_unique_name())
      return nullptr;
  }

  for (xValue* xv : pNode.outputs()) {
    if (!xv->has_unique_name())
      return nullptr;
  }

  // check default attributes
  if (!pNode.hasAttribute(xSymbol("kernel_shape")))
    return nullptr;

  // create operators
  onnc::AveragePool* op = pGraph.addOperator<onnc::AveragePool>(
    pNode.is(xSymbol("kernel_shape")));

  // set default attributes
  SetDefaultAttributes(pNode, *op);

  // set optional attributes
  if (pNode.hasAttribute(xSymbol("auto_pad")))
    op->setAutoPad(pNode.s(xSymbol("auto_pad")));
  if (pNode.hasAttribute(xSymbol("count_include_pad")))
    op->setCountIncludePad(pNode.i(xSymbol("count_include_pad")));
  if (pNode.hasAttribute(xSymbol("pads")))
    op->setPads(pNode.is(xSymbol("pads")));
  if (pNode.hasAttribute(xSymbol("strides")))
    op->setStrides(pNode.is(xSymbol("strides")));

  // set input/output
  for (xValue* xv : pNode.inputs()) {
    onnc::Tensor* tensor = pGraph.getValue<onnc::Tensor>(xv->uniqueName());
    if (nullptr == tensor)
      tensor = IRBuilder::CreateComputeTensor(pGraph, *xv);
    op->addInput(*tensor);
  }

  for (xValue* xv : pNode.outputs()) {
    onnc::Tensor* tensor = pGraph.getValue<onnc::Tensor>(xv->uniqueName());
    if (nullptr == tensor)
      tensor = IRBuilder::CreateComputeTensor(pGraph, *xv);
    op->addOutput(*tensor);
  }
  return op;
}
