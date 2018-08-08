#include "SeetaFace.h"

//二和三
Detector::Detector(const char* model_name) : seeta::FaceDetection(model_name) {
	this->SetMinFaceSize(40);
	this->SetScoreThresh(2.f);
	this->SetImagePyramidScaleFactor(0.8f);
	this->SetWindowStep(4, 4);
}

SeetaFace::SeetaFace() {
	this->detector = new Detector("../model/seeta_fd_frontal_v1.0.bin");
	this->point_detector = new seeta::FaceAlignment("../model/seeta_fa_v1.1.bin");
	this->face_recognizer = new seeta::FaceIdentification("../model/seeta_fr_v1.0.bin");
}

float* SeetaFace::NewFeatureBuffer() {
	return new float[this->face_recognizer->feature_size()];
}

bool SeetaFace::GetFeature(string filename, float* feat) {
	//如果有多张脸，输出第一张脸,把特征放入缓冲区feat，返回true
	//如果没有脸，输出false
	//read pic greyscale
	cv::Mat src_img = cv::imread(filename, 0);
	seeta::ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
	src_img_data.data = src_img.data;

	//read pic color
	cv::Mat src_img_color = cv::imread(filename, 1);
	seeta::ImageData src_img_data_color(src_img_color.cols, src_img_color.rows, src_img_color.channels());
	src_img_data_color.data = src_img_color.data;

	std::vector<seeta::FaceInfo> faces = this->detector->Detect(src_img_data);
	int32_t face_num = static_cast<int32_t>(faces.size());
	if (face_num == 0)
	{
		return false;
	}
	seeta::FacialLandmark points[5];
	this->point_detector->PointDetectLandmarks(src_img_data, faces[0], points);

	this->face_recognizer->ExtractFeatureWithCrop(src_img_data_color, points, feat);

	return true;
};

bool SeetaFace::GetFeatureSingle(string filename, float* feat) {
	//如果有一张脸，提取特征
	//如果有多张脸/没有脸，跳过
	//read pic greyscale
	cv::Mat src_img = cv::imread(filename, 0);
	seeta::ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
	src_img_data.data = src_img.data;

	//read pic color
	cv::Mat src_img_color = cv::imread(filename, 1);
	seeta::ImageData src_img_data_color(src_img_color.cols, src_img_color.rows, src_img_color.channels());
	src_img_data_color.data = src_img_color.data;

	std::vector<seeta::FaceInfo> faces = this->detector->Detect(src_img_data);
	int32_t face_num = static_cast<int32_t>(faces.size());
	if (face_num != 1)
	{
		return false;
	}
	seeta::FacialLandmark points[5];
	this->point_detector->PointDetectLandmarks(src_img_data, faces[0], points);

	this->face_recognizer->ExtractFeatureWithCrop(src_img_data_color, points, feat);

	return true;
};

bool SeetaFace::GetFeatureFromCap(cv::Mat capImg, float* feat) {
	//如果有多张脸，输出第一张脸,把特征放入缓冲区feat，返回true
	//如果没有脸，输出false
	//read pic greyscale
	cv::Mat src_img;
	cv::cvtColor(capImg, src_img, cv::COLOR_BGR2GRAY);
	seeta::ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
	src_img_data.data = src_img.data;

	//read pic color
	seeta::ImageData src_img_data_color(capImg.cols, capImg.rows, capImg.channels());
	src_img_data_color.data = capImg.data;

	std::vector<seeta::FaceInfo> faces = this->detector->Detect(src_img_data);
	int32_t face_num = static_cast<int32_t>(faces.size());
	if (face_num == 0)
	{
		return false;
	}
	seeta::FacialLandmark points[5];
	this->point_detector->PointDetectLandmarks(src_img_data, faces[0], points);

	this->face_recognizer->ExtractFeatureWithCrop(src_img_data_color, points, feat);

	return true;
};

int SeetaFace::GetFeatureDims() {
	return this->face_recognizer->feature_size();
}

float SeetaFace::FeatureCompare(float* feat1, float* feat2) {
	return this->face_recognizer->CalcSimilarity(feat1, feat2);
}

//三
FeatureGroup::FeatureGroup(int feat_dims, SeetaFace* seeta_face) {
	this->feat_dims = feat_dims;
	this->seeta_face = seeta_face;
}


int FeatureGroup::GetFeatureDims() {
	return this->feat_dims;
}


bool FeatureGroup::AddFeature(float* feat, string filename) {
	Feature temp;
	//堆已损坏
	//改了feat的位置，放进for循环中，每次重新创建，否则直接赋值会出问题，因为feat指针地址没变，复制的内容始终是最后一次feat的内容。
	//float* new_feat = new float[this->feat_dims];
	//memcpy(new_feat, feat, sizeof(new_feat) * (this->feat_dims));
	//temp.data = new_feat;
	temp.data = feat;
	temp.filename = filename;
	this->feats.push_back(temp);
	return true;
}


bool FeatureGroup::SaveModel(string model_file) {
	std::ofstream file;
	file.open(model_file);
	file << int(this->feats.size()) << std::endl;
	file << this->feat_dims << std::endl;
	for (int i = 0; i<int(this->feats.size()); i++) {
		file << this->feats[i].filename << std::endl;
		for (int j = 0; j<this->feat_dims; j++)
			file << this->feats[i].data[j] << " ";
		file << std::endl;
	}
	file.close();
	return true;
}


FeatureGroup::FeatureGroup(string model_file, SeetaFace* seeta_face) {
	std::ifstream file;
	file.open(model_file);
	int size;
	float* new_feat;
	char* buffer = new char[1000];
	file >> size;
	file >> this->feat_dims;
	for (int i = 0; i<size; i++) {
		Feature temp;
		file.getline(buffer, 1000);
		while (buffer[0] == '\0' || buffer[0] == ' ') {
			file.getline(buffer, 1000);
		}
		temp.filename = buffer;
		new_feat = new float[this->feat_dims];
		for (int j = 0; j<this->feat_dims; j++)
			file >> new_feat[j];
		temp.data = new_feat;
		this->feats.push_back(temp);
	}
	file.close();
	this->seeta_face = seeta_face;
}


FeatureGroup::~FeatureGroup() {
	for (int i = 0; i<int(this->feats.size()); i++)
		delete[](this->feats[i].data);
}


bool FeatureGroup::FindTopK(int k, float* feat, std::vector<Feature>& result) {
	std::cout << "Calculating Similarities..." << std::endl;
	for (int i = 0; i<int(this->feats.size()); i++) {
		this->feats[i].similarity_with_goal = this->seeta_face->FeatureCompare(this->feats[i].data, feat);
	}
	std::cout << "Finding TopK..." << std::endl;
	std::priority_queue<Feature> q;
	for (int i = 0; i<int(this->feats.size()); i++)
		q.push(this->feats[i]);
	for (int i = 0; i<k; i++) {
		if (q.empty()) return true;
		result.push_back(q.top());
		q.pop();
	}
	return 0;
}


bool GetFilenameUnderPath(string file_path, std::vector<string>& files) {
	//win10:long long  win7:long
	long long hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(file_path).append("/*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetFilenameUnderPath(p.assign(file_path).append("/").append(fileinfo.name), files);
			}
			else
			{
				char *ext = strrchr(fileinfo.name, '.');
				if (ext) {
					ext++;
					if (_stricmp(ext, "jpg") == 0 || _stricmp(ext, "png") == 0)
						files.push_back(p.assign(file_path).append("/").append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

string getFilenameFromPath(string &s) {
	string::size_type pos1 = s.find_last_of('/');
	string::size_type pos2 = s.find_last_of('.');
	string res = s.substr(pos1 + 1, pos2 - pos1 - 1);
	return res;
}

int main(int argc, char* argv[]) {
	int choice;
	std::cout << "input 1 to build face feature library,iuput 2 to test image locally,input 3 call the camera test directly:" << std::endl;
	std::cin >> choice;
	string face_data_path = "../images";
	string face_feat_path = "../feature_model/face_feat_lib.index";

	if (choice == 1) {
		std::vector<string> filenames;
		GetFilenameUnderPath(face_data_path, filenames);
		std::cout << "Detected  " << filenames.size() << "  images...." << std::endl;
		SeetaFace sf;
		FeatureGroup feature_group(sf.GetFeatureDims(), &sf);
		for (int i = 0; i<int(filenames.size()); i++) {
			float* feat = sf.NewFeatureBuffer();
			if (sf.GetFeature(filenames[i], feat)) {
				feature_group.AddFeature(feat, filenames[i]);
			}
			std::cout << (i + 1) << " / " << int(filenames.size()) << std::endl;
		}
		feature_group.SaveModel(face_feat_path);
		std::cout << "Finished." << std::endl;
	}
	else if(choice == 2) {
		SeetaFace sf;
		string pic_file;
		std::cout << "Loading Database..." << std::endl;
		FeatureGroup feature_group(face_feat_path, &sf);
		float* feat = sf.NewFeatureBuffer();
		while (true) {
			std::vector<Feature> result;
			std::cout << "Please Input Your Filename:  ";
			std::cin >> pic_file;
			if (sf.GetFeature(pic_file, feat) == false) {
				std::cout << "Wrong Filename or Can't Detect Face.." << std::endl;
				continue;
			}
			feature_group.FindTopK(10, feat, result);
			for (int i = 0; i<int(result.size()); i++)
				std::cout << "Top " << (i + 1) << " : " << result[i].filename << "  Similarity: " << result[i].similarity_with_goal << std::endl;
		}
		std::cout << std::endl;
	}
	else if (choice == 3) {
		SeetaFace sf;
		std::cout << "Loading Database..." << std::endl;
		FeatureGroup feature_group(face_feat_path, &sf);
		float* feat = sf.NewFeatureBuffer();
		cv::VideoCapture capture(0);
		cv::Mat frame;
		while (true) {
			std::vector<Feature> result;
			std::cout << "opening camera...";
			bool ret = capture.read(frame);
			if (ret) {
				cv::imshow("Face recognition", frame);
				cvWaitKey(30);
				if (sf.GetFeatureFromCap(frame, feat) == false) {
					std::cout << "Can't Detect Face.." << std::endl;
					continue;
				}
				feature_group.FindTopK(3, feat, result);
				for (int i = 0; i<int(result.size()); i++) {
					std::cout << "Top " << (i + 1) << " : " << result[i].filename << "  Similarity: " << result[i].similarity_with_goal << std::endl;
					cv::Mat topK = cv::imread(result[i].filename, 1);
					cv::resize(topK, topK, cv::Size(480, 640));
					cv::Point p = cv::Point(120, 30);
					cv::putText(topK, "Similarity:" + to_string(result[i].similarity_with_goal), p, cv::FONT_HERSHEY_TRIPLEX, 0.8, cv::Scalar(0, 0, 255));
					cv::imshow("top" + to_string(i + 1), topK);
				}
				char c = cvWaitKey(30);
				if (c == 27)break;
			}
		}
		std::cout << std::endl;
		capture.release();
		cv::destroyAllWindows();
	}
	return 0;
}