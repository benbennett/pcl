/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * @author: Koen Buys
 */

#include <pcl/gpu/people/label_common.h>
#include <pcl/gpu/people/probability_processor.h>
#include "internal.h"

pcl::gpu::people::ProbabilityProcessor::ProbabilityProcessor()
{
  std::cout << "(I) : ProbabilityProcessor Constructor called" << std::endl;
  impl_.reset (new device::ProbabilityProc());
}

/** \brief This will merge the votes from the different trees into one final vote, including probabilistic's **/
void
pcl::gpu::people::ProbabilityProcessor::SelectLabel (const Depth& depth, Labels& labels, pcl::device::LabelProbability& probabilities)
{
  std::cout << "(I) : ProbabilityProcessor SelectLabel called" << std::endl;
  impl_->CUDA_SelectLabel(depth, labels, probabilities);
}

/** \brief This will combine two probabilities according their weight **/
void
pcl::gpu::people::ProbabilityProcessor::CombineProb ( const Depth& depth, pcl::device::LabelProbability& probIn1, float weight1,
              pcl::device::LabelProbability& probIn2, float weight2, pcl::device::LabelProbability& probOut)
{
  impl_->CUDA_CombineProb(depth, probIn1, weight1, probIn2, weight2, probOut);
}

/** \brief This will sum a probability multiplied with it's weight **/
void
pcl::gpu::people::ProbabilityProcessor::WeightedSumProb ( const Depth& depth, pcl::device::LabelProbability& probIn, float weight, pcl::device::LabelProbability& probOut)
{
  impl_->CUDA_WeightedSumProb(depth, probIn, weight, probOut);
}

/** \brief This will create a Gaussian Kernel **/
float*
pcl::gpu::people::ProbabilityProcessor::CreateGaussianKernel ( float sigma,
                                                               int kernelSize)
{
  float* f;
  f = (float*) malloc(kernelSize * sizeof(float));
  float sigma_sq = pow(sigma,2);
  float mult = 1/sqrt(2*M_PI*sigma_sq);
  int mid = floor(kernelSize/2);

  // Create a symmetric kernel, could also be solved in CUDA kernel but let's do it here :D
  for(int i = 0; i < kernelSize; i++)
  {
    f[i] = mult * exp(-(pow(i-mid,2)/2*sigma_sq));
  }
  return f;
}
