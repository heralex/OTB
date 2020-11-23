/*
 * Copyright (C) 2005-2020 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"
#include "otbOGRDataSourceWrapper.h"
#include "otbSampleAugmentationFilter.h"

namespace otb
{
namespace Wrapper
{


class SampleAugmentation : public Application
{
public:
  /** Standard class typedefs. */
  typedef SampleAugmentation            Self;
  typedef Application                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(SampleAugmentation, otb::Application);

  /** Filters typedef */
  using FilterType       = otb::SampleAugmentationFilter;
  using SampleType       = FilterType::SampleType;
  using SampleVectorType = FilterType::SampleVectorType;

private:
  SampleAugmentation()
  {
  }

  void DoInit() override
  {
    SetName("SampleAugmentation");
    SetDescription("Generates synthetic samples from a sample data file.");

    // Documentation
    SetDocLongDescription(
        "The application takes a sample data file as "
        "generated by the SampleExtraction application and "
        "generates synthetic samples to increase the number of "
        "available samples.");
    SetDocLimitations("None");
    SetDocAuthors("OTB-Team");
    SetDocSeeAlso(" ");

    AddDocTag(Tags::Learning);

    AddParameter(ParameterType_InputVectorData, "in", "Input samples");
    SetParameterDescription("in", "Vector data file containing samples (OGR format)");

    AddParameter(ParameterType_OutputFilename, "out", "Output samples");
    SetParameterDescription("out",
                            "Output vector data file storing new samples"
                            "(OGR format).");

    AddParameter(ParameterType_Field, "field", "Field Name");
    SetParameterDescription("field", "Name of the field carrying the class name in the input vectors.");
    SetListViewSingleSelectionMode("field", true);
    SetVectorData("field", "in");
    SetTypeFilter("field", { OFTString, OFTInteger, OFTInteger64 });

    AddParameter(ParameterType_Int, "layer", "Layer Index");
    SetParameterDescription("layer", "Layer index to read in the input vector file.");
    MandatoryOff("layer");
    SetDefaultParameterInt("layer", 0);

    AddParameter(ParameterType_Int, "label", "Label of the class to be augmented");
    SetParameterDescription("label",
                            "Label of the class of the input file for which "
                            "new samples will be generated.");
    SetDefaultParameterInt("label", 1);

    AddParameter(ParameterType_Int, "samples", "Number of generated samples");
    SetParameterDescription("samples",
                            "Number of synthetic samples that will "
                            "be generated.");
    SetDefaultParameterInt("samples", 100);

    AddParameter(ParameterType_Field, "exclude", "Field names for excluded features");
    SetParameterDescription("exclude", "List of field names in the input vector data that will not be generated in the output file.");
    SetVectorData("exclude", "in");

    AddParameter(ParameterType_Choice, "strategy", "Augmentation strategy");

    AddChoice("strategy.replicate", "Replicate input samples");
    SetParameterDescription("strategy.replicate",
                            "The new samples are generated "
                            "by replicating input samples which are randomly "
                            "selected with replacement.");

    AddChoice("strategy.jitter", "Jitter input samples");
    SetParameterDescription("strategy.jitter",
                            "The new samples are generated "
                            "by adding gaussian noise to input samples which are "
                            "randomly selected with replacement.");
    AddParameter(ParameterType_Float, "strategy.jitter.stdfactor", "Factor for dividing the standard deviation of each feature");
    SetParameterDescription("strategy.jitter.stdfactor",
                            "The noise added to the input samples will have the "
                            "standard deviation of the input features divided "
                            "by the value of this parameter. ");
    SetDefaultParameterFloat("strategy.jitter.stdfactor", 10);

    AddChoice("strategy.smote", "Smote input samples");
    SetParameterDescription("strategy.smote",
                            "The new samples are generated "
                            "by using the SMOTE algorithm (http://dx.doi.org/10.1613/jair.953) "
                            "on input samples which are "
                            "randomly selected with replacement.");
    AddParameter(ParameterType_Int, "strategy.smote.neighbors", "Number of nearest neighbors");
    SetParameterDescription("strategy.smote.neighbors",
                            "Number of nearest neighbors to be used in the "
                            "SMOTE algorithm");
    SetDefaultParameterInt("strategy.smote.neighbors", 5);

    AddRANDParameter("seed");
    MandatoryOff("seed");

    // Doc example parameter settings
    SetDocExampleParameterValue("in", "samples.sqlite");
    SetDocExampleParameterValue("field", "class");
    SetDocExampleParameterValue("label", "3");
    SetDocExampleParameterValue("samples", "100");
    SetDocExampleParameterValue("out", "augmented_samples.sqlite");
    SetDocExampleParameterValue("exclude", "OGC_FID name class originfid");
    SetDocExampleParameterValue("strategy", "smote");
    SetDocExampleParameterValue("strategy.smote.neighbors", "5");

    SetOfficialDocLink();
  }

  void DoUpdateParameters() override
  {
    if (HasValue("in"))
    {
      std::string              vectorFile = GetParameterString("in");
      ogr::DataSource::Pointer ogrDS      = ogr::DataSource::New(vectorFile, ogr::DataSource::Modes::Read);
      ogr::Layer               layer      = ogrDS->GetLayer(this->GetParameterInt("layer"));
      ogr::Feature             feature    = layer.ogr().GetNextFeature();

      ClearChoices("exclude");
      ClearChoices("field");

      FieldParameter::TypeFilterType typeFilter = GetTypeFilter("field");
      for (int iField = 0; iField < feature.ogr().GetFieldCount(); iField++)
      {
        std::string key, item = feature.ogr().GetFieldDefnRef(iField)->GetNameRef();
        key                       = item;
        std::string::iterator end = std::remove_if(key.begin(), key.end(), [](auto c) { return !std::isalnum(c); });
        std::transform(key.begin(), end, key.begin(), tolower);

        OGRFieldType fieldType = feature.ogr().GetFieldDefnRef(iField)->GetType();

        if (std::find(typeFilter.begin(), typeFilter.end(), fieldType) != std::end(typeFilter))
        {
          std::string tmpKey = "field." + key.substr(0, end - key.begin());
          AddChoice(tmpKey, item);
        }

        std::string tmpKey = "exclude." + key.substr(0, static_cast<unsigned long>(end - key.begin()));
        AddChoice(tmpKey, item);
      }
    }
  }

  void DoExecute() override
  {
    ogr::DataSource::Pointer vectors;
    ogr::DataSource::Pointer output;
    vectors = ogr::DataSource::New(this->GetParameterString("in"));
    output  = ogr::DataSource::New(this->GetParameterString("out"), ogr::DataSource::Modes::Overwrite);

    // Retrieve the field name
    std::vector<int> selectedCFieldIdx = GetSelectedItems("field");

    if (selectedCFieldIdx.empty())
    {
      otbAppLogFATAL(<< "No field has been selected for data labelling!");
    }

    std::vector<std::string> cFieldNames = GetChoiceNames("field");
    std::string              fieldName   = cFieldNames[selectedCFieldIdx.front()];

    std::vector<std::string> excludedFields = GetExcludedFields(GetChoiceNames("exclude"), GetSelectedItems("exclude"));
    for (const auto& ef : excludedFields)
      otbAppLogINFO("Excluding feature " << ef << '\n');

    int seed = std::time(nullptr);
    if (IsParameterEnabled("seed"))
      seed = this->GetParameterInt("seed");


    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(vectors);
    filter->SetLayer(this->GetParameterInt("layer"));
    filter->SetNumberOfSamples(this->GetParameterInt("samples"));
    filter->SetOutputSamples(output);
    filter->SetClassFieldName(fieldName);
    filter->SetLabel(this->GetParameterInt("label"));
    filter->SetExcludedFields(excludedFields);
    filter->SetSeed(seed);
    switch (this->GetParameterInt("strategy"))
    {
    // replicate
    case 0:
    {
      otbAppLogINFO("Augmentation strategy : replicate");
      filter->SetStrategy(FilterType::Strategy::Replicate);
    }
    break;
    // jitter
    case 1:
    {
      otbAppLogINFO("Augmentation strategy : jitter");
      filter->SetStrategy(FilterType::Strategy::Jitter);
      filter->SetStdFactor(this->GetParameterFloat("strategy.jitter.stdfactor"));
    }
    break;
    case 2:
    {
      otbAppLogINFO("Augmentation strategy : smote");
      filter->SetStrategy(FilterType::Strategy::Smote);
      filter->SetSmoteNeighbors(this->GetParameterInt("strategy.smote.neighbors"));
    }
    break;
    }
    filter->Update();
    output->SyncToDisk();
  }


  std::vector<std::string> GetExcludedFields(const std::vector<std::string>& fieldNames, const std::vector<int>& selectedIdx)
  {
    auto                     nbFeatures = static_cast<unsigned int>(selectedIdx.size());
    std::vector<std::string> result(nbFeatures);
    for (unsigned int i = 0; i < nbFeatures; ++i)
    {
      result[i] = fieldNames[selectedIdx[i]];
    }
    return result;
  }
};

} // end of namespace Wrapper
} // end of namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::SampleAugmentation)
