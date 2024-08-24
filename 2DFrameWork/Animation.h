#pragma once

enum class AnimationState
{
	STOP,//Pause
	LOOP,
	ONCE_LAST,
	ONCE_FIRST,
};

class Animation
{
public:
	UINT					frameMax;
	UINT					boneMax;
	Matrix** arrFrameBone;//	프레임갯수* 본갯수 ( local Inverse * ani)
	float					tickPerSecond;
	string					file;


	Animation();
	~Animation();
	void LoadFile(string file);
	void SaveFile(string file);
};


class Animations
{
	struct Animator
	{
		float frameWeight = 0.0f;
		UINT  currentFrame = 0;
		UINT  nextFrame = 1;
		UINT  animIdx = 0;
		AnimationState animState = AnimationState::STOP;
	};
	void AnimatorUpdate(Animator& Animator);
public:
	Animations();
	~Animations();
	void Update();

	//현재 애니메이션에서 다음 애니메이션으로 넘어갈 애니메이터
	Animator							currentAnimator;
	Animator							nextAnimator;

	bool								isChanging;
	float								blendtime;
	float								Changedtime;
	float								aniScale = 1.0f;
	vector<shared_ptr<Animation>>		playList;
	Matrix	GetFrameBone(int boneIndex);
	void	ChangeAnimation(AnimationState state, UINT idx, float blendtime = 0.0f);
	void	RenderDetail();
	float   GetPlayTime();// 0처음 ~ 1 끝
};