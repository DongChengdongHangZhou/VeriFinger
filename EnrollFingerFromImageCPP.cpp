#include <TutorialUtils.hpp>

#ifdef N_MAC_OSX_FRAMEWORKS
	#include <NCore/NCore.hpp>
	#include <NBiometricClient/NBiometricClient.hpp>
	#include <NBiometrics/NBiometrics.hpp>
	#include <NMedia/NMedia.hpp>
	#include <NLicensing/NLicensing.hpp>
#else
	#include <NCore.hpp>
	#include <NBiometricClient.hpp>
	#include <NBiometrics.hpp>
	#include <NMedia.hpp>
	#include <NLicensing.hpp>
#endif
#include <iostream>
#include <fstream>

using namespace std;
using namespace Neurotec;
using namespace Neurotec::Licensing;
using namespace Neurotec::Biometrics;
using namespace Neurotec::Biometrics::Client;
using namespace Neurotec::Biometrics::Standards;
using namespace Neurotec::IO;
using namespace Neurotec::Images;

const NChar title[] = N_T("EnrollFingerFromImage");
const NChar description[] = N_T("Demonstrates fingerprint feature extraction from image.");
const NChar version[] = N_T("12.3.0.0");
const NChar copyright[] = N_T("Copyright (C) 2016-2022 Neurotechnology");

int usage()
{
	cout << "usage:" << endl;
	cout << "\t" << title << " [image] [template] [format]" << endl << endl;
	cout << "\t[image]    - image filename to extract." << endl;
	cout << "\t[template] - filename to store extracted features." << endl;
	cout << "\t[format]   - whether proprietary or standard template should be created." << endl;
	cout << "\t\tIf not specified, proprietary Neurotechnology template is created (recommended)." << endl;
	cout << "\t\tANSI for ANSI/INCITS 378-2004" << endl;
	cout << "\t\tISO for ISO/IEC 19794-2" << endl << endl << endl;
	cout << "examples:" << endl;
	cout << "\t" << title << " image.jpg template.dat" << endl;
	cout << "\t" << title << " image.jpg isoTemplate.dat ISO" << endl;
	return 1;
}

int main(int argc, NChar **argv)
{
	OnStart(title, description, version, copyright, argc, argv);

	BdifStandard standard = bsUnspecified;

	if (argc < 3)
	{
		OnExit();
		return usage();
	}
	if (argc > 3)
	{
		if (!strcmp(argv[3], N_T("ANSI")))
		{
			standard = bsAnsi;
		}
		else if (!strcmp(argv[3], N_T("ISO")))
		{
			standard = bsIso;
		}
	}

	//=========================================================================
	// CHOOSE LICENCES !!!
	//=========================================================================
	// ONE of the below listed licenses is required for unlocking this sample's functionality. Choose a license that you currently have on your device.
	// If you are using a TRIAL version - choose any of them.

	const NString license = N_T("FingerExtractor");
	//const NString license = N_T("FingerClient");
	//const NString license = N_T("FingerFastExtractor");

	//=========================================================================

	//=========================================================================
	// TRIAL MODE
	//=========================================================================
	// Below code line determines whether TRIAL is enabled or not. To use purchased licenses, don't use below code line.
	// GetTrialModeFlag() method takes value from "Bin/Licenses/TrialFlag.txt" file. So to easily change mode for all our examples, modify that file.
	// Also you can just set TRUE to "TrialMode" property in code.

	NLicenseManager::SetTrialMode(GetTrialModeFlag());
	cout << "Trial mode: " << NLicenseManager::GetTrialMode() << endl;

	//=========================================================================

	try
	{
		// Obtain license
		if (!NLicense::Obtain(N_T("/local"), N_T("5000"), license))
		{
			NThrowException(NString::Format(N_T("Could not obtain license: {S}"), license.GetBuffer())); 
		}

		NBiometricClient biometricClient;
		biometricClient.SetFingersTemplateSize(ntsLarge);

		NSubject subject;
		NFinger finger;

		NImage image = NImage::FromFile(argv[1]);
		finger.SetImage(image);
		subject.GetFingers().Add(finger);
		NBiometricStatus status = biometricClient.CreateTemplate(subject);

		if (status == nbsOk)
		{
			cout << "Template extracted: " << endl;

			NFingerExaminer examiner;
			examiner.InitializeFromOriginalImage(biometricClient, image);
			NTemplate ntemplate = subject.GetTemplate();
			examiner.FindSingularPoints(ntemplate.GetFingers().GetRecords()[0]);

			cout << "Singular points extracted extracted: " << endl;
			
			ofstream outfile("data.txt");
			NFloat angle;
			NUShort x;
			NUShort y;
			NByte quality;
			NByte type;
			NInt num(subject.GetTemplate().GetFingers().GetRecords()[0].GetMinutiae().GetCount());
			outfile << subject.GetTemplate().GetFingers().GetRecords()[0].GetCores()[0].X << " " << subject.GetTemplate().GetFingers().GetRecords()[0].GetCores()[0].Y << " " << subject.GetTemplate().GetFingers().GetRecords()[0].GetCores()[0].Angle << endl;
			outfile << subject.GetTemplate().GetFingers().GetRecords()[0].GetDeltas()[0].X << " " << subject.GetTemplate().GetFingers().GetRecords()[0].GetDeltas()[0].Y << endl;
			for (int i = 0;i < num;i++) {
				angle = NBiometricTypes::AngleToDegrees(subject.GetTemplate().GetFingers().GetRecords()[0].GetMinutiae()[i].Angle);
				type = subject.GetTemplate().GetFingers().GetRecords()[0].GetMinutiae()[i].Type;
				quality = subject.GetTemplate().GetFingers().GetRecords()[0].GetMinutiae()[i].Quality;
				x = subject.GetTemplate().GetFingers().GetRecords()[0].GetMinutiae()[i].X;
				y = subject.GetTemplate().GetFingers().GetRecords()[0].GetMinutiae()[i].Y;
				outfile << x << " " << y << " " << angle << " " << type << " " << quality << endl;
			}
			outfile.close();

			if (standard == bsIso)
			{
				cout << "ISO" << endl;
				NFile::WriteAllBytes(argv[2], subject.GetTemplateBuffer(CBEFF_BO_ISO_IEC_JTC_1_SC_37_BIOMETRICS, CBEFF_BDBFI_ISO_IEC_JTC_1_SC_37_BIOMETRICS_FINGER_MINUTIAE_RECORD_FORMAT, FMR_VERSION_ISO_CURRENT));
			}
			else if (standard == bsAnsi)
			{
				cout << "ANSI" << endl;
				NFile::WriteAllBytes(argv[2], subject.GetTemplateBuffer(CBEFF_BO_INCITS_TC_M1_BIOMETRICS, CBEFF_BDBFI_INCITS_TC_M1_BIOMETRICS_FINGER_MINUTIAE_U, FMR_VERSION_ANSI_CURRENT));
			}
			else
			{
				cout << "Proprietary" << endl;
				NFile::WriteAllBytes(argv[2], subject.GetTemplateBuffer());
			}
			cout << "Template saved successfully" << endl;
		}
		else
		{
			cout << "Extraction failed: " << NEnum::ToString(NBiometricTypes::NBiometricStatusNativeTypeOf(), status) << endl;
		}
	}
	catch (NError& ex)
	{
		return LastError(ex);
	}

	OnExit();
	return 0;
}

