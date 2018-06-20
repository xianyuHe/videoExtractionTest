#pragma once
#include "include\cmpvdeco.h"
#include <tchar.h>
#include <dshow.h>
#include "include\convert.h"
#using <mscorlib.dll> 
#using <System.Drawing.dll>


struct DCTBlockStruct
{
	short A1, A2, A3, A4, B1, B2, B3, B4, C11;
} typedef ABBlocks;

namespace face
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	using namespace System::Runtime::InteropServices;

	WCHAR wpFileName[200];


	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
		}
		~Form1(void)
		{
			if (components)
				delete components;
		}

	private: System::ComponentModel::IContainer ^  components;
	private: System::Windows::Forms::PictureBox ^  pictureBox;
	private: System::Windows::Forms::Button ^  startButton;
	private: System::Windows::Forms::OpenFileDialog ^  openFileDialog;

	private:
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			//picture box
			this->pictureBox = (gcnew System::Windows::Forms::PictureBox());
			pictureBox->Location = Point(266, 16);
			pictureBox->Size = System::Drawing::Size(200, 150);
			//pictureBox->Image = Image::FromFile(L"C:\\Users\\75612\\Pictures\\Saved Pictures\\rename.png");


			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			//button
			this->startButton = (gcnew System::Windows::Forms::Button());
			this->startButton->Location = System::Drawing::Point(470, 600);
			this->startButton->Size = System::Drawing::Size(271, 69);
			this->startButton->TabIndex = 3;
			this->startButton->Text = L"Extract video";
			this->startButton->Click += gcnew System::EventHandler(this, &Form1::startButton_click);

			this->Controls->Add(this->pictureBox);
			this->Controls->Add(this->startButton);

			this->AutoScaleBaseSize = System::Drawing::Size(8, 19);
			this->ClientSize = System::Drawing::Size(1104, 857);
		}



	private: System::Void startButton_click(System::Object ^  sender, System::EventArgs ^  e)
	{
		const HEADERDATA *pMpvHeaderData;
		char psFileName[200];
		CMpvDecoder MpvDecoder;
		WORD wYPicWidth, wYPicHeight;
		long lResult;
		long lastnum;
		BYTE type1;
		long crt_num;
		double block_num;
		COMPRESSEDPIC *pCompressedPic;
		BYTE* pYBuff;
		BYTE* pCbBuff;
		BYTE* pCrBuff;
		int i;
		int i_number;

		if (this->openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			int i = this->openFileDialog->FileName->Length;
			for (int j = 0; j<i; j++)
			{
				psFileName[j] = this->openFileDialog->FileName->ToCharArray()[j];
			}
			for (int j = i; j < 200; j++)
			{
				psFileName[j] = 0;
			}
			if ((lResult = MpvDecoder.Initialize(psFileName)) != MDC_SUCCESS)
				return;
		}
		MpvDecoder.GetMaxFrameNum(&lastnum);

		pMpvHeaderData = MpvDecoder.GetMpvHeaders();
		SEQUENCEHEADER sequencehead = pMpvHeaderData->SequenceHeader;
		//帧的水平长度/16，怕不是一个帧有16个ypic
		wYPicWidth = sequencehead.wHorizontalSize / 16;
		wYPicHeight = sequencehead.wVerticalSize / 16;
		block_num = wYPicWidth * wYPicHeight;
		i_number = 0;


		System::Drawing::Rectangle rc;
		rc = this->ClientRectangle;
		//int num_row = (int)(rc.Height / wYPicHeight);
		//int num_col = (int)(rc.Width / wYPicWidth);
		//一个rec里能显示多少个ypic
		//int num_pic = num_row * num_col;
		//array<System::Windows::Forms::PictureBox^>^picBox = gcnew array<System::Windows::Forms::PictureBox^>(num_pic);
		//for (i = 0; i<num_pic; i++)
		//{
		//	int heightLoc = (int)(num_pic / num_col);
		//	int widthLoc = num_pic % num_col;
		//	picBox[i] = gcnew System::Windows::Forms::PictureBox();
		//	picBox[i]->Location = System::Drawing::Point(heightLoc, widthLoc);
		//	picBox[i]->Size = System::Drawing::Size(wYPicHeight, wYPicWidth);
		//}



		System::Drawing::Bitmap^ bitmap1 = gcnew System::Drawing::Bitmap((int)(wYPicWidth), (int)(wYPicHeight),
			System::Drawing::Imaging::PixelFormat::Format24bppRgb);

		pYBuff = new BYTE[wYPicWidth*wYPicHeight*4];
		pCrBuff = new BYTE[wYPicWidth*wYPicHeight];
		pCbBuff = new BYTE[wYPicWidth*wYPicHeight];

		pCompressedPic = MpvDecoder.PrepareCompressedPic();

	/*	const double INVERSE_TABLE_A[16][9] =
		{
			{0.125, 0.176, 0.125, 0.176, 0.176, 0.176, 0.125, 0.176, 0.125},
			{0.125, 0.06, -0.125, 0.176, 0.0625, -0.176, 0.125, 0.0625, -0.125},
			{0.125, -0.06, -0.125, 0.176, -0.0625, -0.176, 0.125, -0.0625, -0.125},
			{0.125, -0.176, 0.125, -0.176, -0.176, 0.176, 0.125, -0.176, 0.125},
			{0.125, 0.176, 0.125, 0.0625, 0.0625, 0.0625, 0.125, -0.176, -0.125}.
			{0.125, 0.06, -0.125, 0.0625, 0, -0.0625, -0.125, -0.0625, }
		}*/
		//重构第零帧
		for (i = 0; i < (int)(lastnum - 1); i++)
		{

			lResult = MpvDecoder.GetCompressedPic();
			pMpvHeaderData = MpvDecoder.GetMpvHeaders();
			type1 = pMpvHeaderData->PictureHeader.nPictureCodingType;

			//MpvDecoder.GetCurFrameNum(&crt_num);

			if (type1 == I_TYPE)
			{
				int j;

				System::Drawing::Rectangle^ rect2 = gcnew System::Drawing::Rectangle(0, 0, (int)wYPicWidth, (int)wYPicHeight);

				System::Drawing::Imaging::BitmapData^ bmData = bitmap1->LockBits(
					*rect2,
					System::Drawing::Imaging::ImageLockMode::ReadWrite,
					System::Drawing::Imaging::PixelFormat::Format24bppRgb);


				int m_stride = bmData->Stride;
				int m_scan0 = bmData->Scan0.ToInt32();
				
					
				//YDCT block
				DCTBLOCK *C = pCompressedPic->pshYDCTBlock;
				ABBlocks AB;
				assignBlock(C, AB);
				for(int i = 0; i < 4; i++){}
				pYBuff[0] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B3 + AB.B1 + AB.C11) >> 3) + ((AB.B3 + AB.B1 + AB.C11) >> 5));
				pYBuff[1] = (BYTE)(((AB.A2 + AB.A4) >> 3) + (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) + (AB.C11 >> 4));
				pYBuff[2] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) - (AB.C11 >> 4));
				pYBuff[3] = (BYTE)(((AB.A1 + AB.A3) >> 3) - ((AB.B3 - AB.B1) >> 4) - (AB.C11 >> 4));
				pYBuff[4] = (BYTE)(((AB.A1 - AB.A3) >> 3) + (AB.B3 >> 3) + (AB.B3 >> 5) + (AB.B1 >> 4) + (AB.C11 >> 4));
				pYBuff[5] = (BYTE)(((AB.A2 - AB.A4) >> 3) + ((AB.B4 + AB.B2) >> 4));
				pYBuff[6] = (BYTE)(((AB.A2 - AB.A4) >> 3) - ((AB.B4 - AB.B2) >> 4));
				pYBuff[7] = (BYTE)(((AB.A1 - AB.A3) >> 3) - (AB.B4 >> 3) + (AB.B4 >> 5) + (AB.B1 >> 4) + (AB.C11 >> 4));
				pYBuff[8] = (BYTE)(((AB.A1 - AB.A3) >> 3) + (AB.B4 >> 3) + (AB.B4 >> 5) - (AB.B1 >> 4) - (AB.C11 >> 4));
				pYBuff[9] = (BYTE)(((AB.A2 - AB.A4) >> 3) + (AB.B4 >> 4) - (AB.B2 >> 4));
				pYBuff[10] = (BYTE)(((AB.A2 - AB.A4) >> 3) - ((AB.B4 + AB.B2) >> 4));
				pYBuff[11] = (BYTE)(((AB.A1 - AB.A3) >> 3) - (AB.B4 >> 3) + (AB.B4 >> 5) - (AB.B1 >> 4) + (AB.C11 >> 4));
				pYBuff[12] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 5));
				pYBuff[13] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) - (AB.C11 >> 4));
				pYBuff[14] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) - (AB.B3 >> 4) + (AB.C11 >> 4));
				pYBuff[15] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 5));



				//CbDCT block
				C = pCompressedPic->pshCbDCTBlock;
				assignBlock(C, AB);
				pCbBuff[0] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B3 + AB.B1 + AB.C11) >> 3) + ((AB.B3 + AB.B1 + AB.C11) >> 5));
				pCbBuff[1] = (BYTE)(((AB.A2 + AB.A4) >> 3) + (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) + (AB.C11 >> 4));
				pCbBuff[2] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) - (AB.C11 >> 4));
				pCbBuff[3] = (BYTE)(((AB.A1 + AB.A3) >> 3) - ((AB.B3 - AB.B1) >> 4) - (AB.C11 >> 4));
				pCbBuff[4] = (BYTE)(((AB.A1 - AB.A3) >> 3) + (AB.B3 >> 3) + (AB.B3 >> 5) + (AB.B1 >> 4) + (AB.C11 >> 4));
				pCbBuff[5] = (BYTE)(((AB.A2 - AB.A4) >> 3) + ((AB.B4 + AB.B2) >> 4));
				pCbBuff[6] = (BYTE)(((AB.A2 - AB.A4) >> 3) - ((AB.B4 - AB.B2) >> 4));
				pCbBuff[7] = (BYTE)(((AB.A1 - AB.A3) >> 3) - (AB.B4 >> 3) + (AB.B4 >> 5) + (AB.B1 >> 4) + (AB.C11 >> 4));
				pCbBuff[8] = (BYTE)(((AB.A1 - AB.A3) >> 3) + (AB.B4 >> 3) + (AB.B4 >> 5) - (AB.B1 >> 4) - (AB.C11 >> 4));
				pCbBuff[9] = (BYTE)(((AB.A2 - AB.A4) >> 3) + (AB.B4 >> 4) - (AB.B2 >> 4));
				pCbBuff[10] = (BYTE)(((AB.A2 - AB.A4) >> 3) - ((AB.B4 + AB.B2) >> 4));
				pCbBuff[11] = (BYTE)(((AB.A1 - AB.A3) >> 3) - (AB.B4 >> 3) + (AB.B4 >> 5) - (AB.B1 >> 4) + (AB.C11 >> 4));
				pCbBuff[12] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 5));
				pCbBuff[13] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) - (AB.C11 >> 4));
				pCbBuff[14] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) - (AB.B3 >> 4) + (AB.C11 >> 4));
				pCbBuff[15] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 5));




				//CrDCT block
				C = pCompressedPic->pshCrDCTBlock;
				assignBlock(C, AB);
				pCrBuff[0] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B3 + AB.B1 + AB.C11) >> 3) + ((AB.B3 + AB.B1 + AB.C11) >> 5));
				pCrBuff[1] = (BYTE)(((AB.A2 + AB.A4) >> 3) + (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) + (AB.C11 >> 4));
				pCrBuff[2] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) - (AB.C11 >> 4));
				pCrBuff[3] = (BYTE)(((AB.A1 + AB.A3) >> 3) - ((AB.B3 - AB.B1) >> 4) - (AB.C11 >> 4));
				pCrBuff[4] = (BYTE)(((AB.A1 - AB.A3) >> 3) + (AB.B3 >> 3) + (AB.B3 >> 5) + (AB.B1 >> 4) + (AB.C11 >> 4));
				pCrBuff[5] = (BYTE)(((AB.A2 - AB.A4) >> 3) + ((AB.B4 + AB.B2) >> 4));
				pCrBuff[6] = (BYTE)(((AB.A2 - AB.A4) >> 3) - ((AB.B4 - AB.B2) >> 4));
				pCrBuff[7] = (BYTE)(((AB.A1 - AB.A3) >> 3) - (AB.B4 >> 3) + (AB.B4 >> 5) + (AB.B1 >> 4) + (AB.C11 >> 4));
				pCrBuff[8] = (BYTE)(((AB.A1 - AB.A3) >> 3) + (AB.B4 >> 3) + (AB.B4 >> 5) - (AB.B1 >> 4) - (AB.C11 >> 4));
				pCrBuff[9] = (BYTE)(((AB.A2 - AB.A4) >> 3) + (AB.B4 >> 4) - (AB.B2 >> 4));
				pCrBuff[10] = (BYTE)(((AB.A2 - AB.A4) >> 3) - ((AB.B4 + AB.B2) >> 4));
				pCrBuff[11] = (BYTE)(((AB.A1 - AB.A3) >> 3) - (AB.B4 >> 3) + (AB.B4 >> 5) - (AB.B1 >> 4) + (AB.C11 >> 4));
				pCrBuff[12] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 5));
				pCrBuff[13] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) + (AB.B3 >> 4) - (AB.C11 >> 4));
				pCrBuff[14] = (BYTE)(((AB.A2 + AB.A4) >> 3) - (AB.B2 >> 3) + (AB.B2 >> 5) - (AB.B3 >> 4) + (AB.C11 >> 4));
				pCrBuff[15] = (BYTE)(((AB.A1 + AB.A3) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 3) + ((AB.B4 - AB.B1 - AB.C11) >> 5));



				//}

				bitmap1->UnlockBits(bmData);
				this->pictureBox->Image = bitmap1;
				this->pictureBox->Update();
				this->Controls->Add(this->pictureBox);
				i_number++;
			}

			if (MpvDecoder.MoveToNextFrame() != MDC_SUCCESS) {
				return;
			}

		}
		

		MpvDecoder.FreeCompressedPic();
	}

	void assignBlock(DCTBLOCK * C, ABBlocks& AB)
	{

		AB.A1 = C[0][0] + C[0][16];
		AB.A2 = C[0][0] - C[0][16];
		AB.A3 = C[0][2] + C[0][18];
		AB.A4 = C[0][2] - C[0][18];
		AB.B1 = C[0][1] + C[0][17];
		AB.B2 = C[0][1] - C[0][17];
		AB.B3 = C[0][1] + C[0][10];
		AB.B4 = C[0][1] - C[0][10];
		AB.C11 = C[0][9];
	}
	};


}