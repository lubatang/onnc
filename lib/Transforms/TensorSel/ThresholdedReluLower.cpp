//===- ThresholdedReluLower.cpp -------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <onnc/Transforms/TensorSel/Lower.h>
#include <onnc/Transforms/TensorSel/Standards/ThresholdedReluLower.h>
#include <onnc/IR/Compute/ThresholdedRelu.h>
#include "SetDefaultAttributes.h"
#include <onnc/IR/IRBuilder.h>

using namespace onnc;

//===----------------------------------------------------------------------===//
// ThresholdedReluLower
//===----------------------------------------------------------------------===//
ThresholdedReluLower::ThresholdedReluLower()
{
}

ThresholdedReluLower::~ThresholdedReluLower()
{
}

int ThresholdedReluLower::isMe(const xNode& pNode) const
{
  if (pNode.kind() == xSymbol("ThresholdedRelu"))
    return kStdLower;
  return kNotMe;
}

ComputeOperator*
ThresholdedReluLower::activate(ComputeGraph& pGraph, xNode& pNode) const
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
  

  // create operators
  onnc::ThresholdedRelu* op = pGraph.addOperator<onnc::ThresholdedRelu>();

  // set default attributes
  SetDefaultAttributes(pNode, *op);

  // set optional attributes
  if (pNode.hasAttribute(xSymbol("alpha")))
    op->setAlpha(pNode.f(xSymbol("alpha")));

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
