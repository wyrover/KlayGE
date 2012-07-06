#include <KlayGE/KlayGE.hpp>
#include <KlayGE/ResLoader.hpp>
#include <KlayGE/Context.hpp>
#include <KlayGE/FrameBuffer.hpp>
#include <KlayGE/RenderFactory.hpp>
#include <KlayGE/InputFactory.hpp>
#include <KlayGE/RenderEngine.hpp>
#include <KlayGE/RenderEffect.hpp>
#include <KlayGE/SceneObject.hpp>
#include <KlayGE/SceneObjectHelper.hpp>
#include <KlayGE/Camera.hpp>
#include <KlayGE/Mesh.hpp>
#include <KlayGE/PostProcess.hpp>
#include <KlayGE/Light.hpp>
#include <KlayGE/DeferredRenderingLayer.hpp>
#include <KlayGE/UI.hpp>
#include <KlayGE/RenderableHelper.hpp>
#include <KlayGE/SceneManager.hpp>
#include <KlayGE/Window.hpp>

#include <KlayGE/SSRPostProcess.hpp>

#include <sstream>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>

#include "AtmosphericScattering.hpp"

using namespace KlayGE;

namespace
{
	class PlanetMesh : public StaticMesh
	{
	public:
		PlanetMesh(RenderModelPtr const & model, std::wstring const & name)
			: StaticMesh(model, name)
		{
			RenderFactory & rf = Context::Instance().RenderFactoryInstance();
			RenderEffectPtr effect = rf.LoadEffect("AtmosphericScattering.fxml");
			technique_ = effect->TechniqueByName("PlanetTech");
		}

		void BuildMeshInfo()
		{
		}

		void Density(float density)
		{
			*(technique_->Effect().ParameterByName("density")) = density;
		}

		void Beta(Color const & clr)
		{
			*(technique_->Effect().ParameterByName("beta")) = float3(clr.r(), clr.g(), clr.b());
		}

		void Absorb(Color const & clr)
		{
			*(technique_->Effect().ParameterByName("absorb")) = float3(clr.r(), clr.g(), clr.b());
		}
		
		void OnRenderBegin()
		{
			App3DFramework const & app = Context::Instance().AppInstance();

			float4x4 const & view = app.ActiveCamera().ViewMatrix();
			float4x4 const & proj = app.ActiveCamera().ProjMatrix();

			*(technique_->Effect().ParameterByName("mvp")) = model_mat_ * view * proj;

			float4x4 inv_mv = MathLib::inverse(model_mat_ * view);
			*(technique_->Effect().ParameterByName("eye_pos")) = MathLib::transform_coord(float3(0, 0, 0), inv_mv);
			*(technique_->Effect().ParameterByName("look_at_vec")) = MathLib::transform_normal(float3(0, 0, 1), inv_mv);
		}
	};

	class AtmosphereMesh : public StaticMesh
	{
	public:
		AtmosphereMesh(RenderModelPtr const & model, std::wstring const & name)
			: StaticMesh(model, name)
		{
			RenderFactory & rf = Context::Instance().RenderFactoryInstance();
			RenderEffectPtr effect = rf.LoadEffect("AtmosphericScattering.fxml");
			technique_ = effect->TechniqueByName("AtmosphereTech");
		}

		void BuildMeshInfo()
		{
			aabb_.Min() *= 1.2f;
			aabb_.Max() *= 1.2f;
		}

		void AtmosphereTop(float top)
		{
			*(technique_->Effect().ParameterByName("atmosphere_top")) = top;
		}

		void Density(float density)
		{
			*(technique_->Effect().ParameterByName("density")) = density;
		}

		void Beta(Color const & clr)
		{
			*(technique_->Effect().ParameterByName("beta")) = float3(clr.r(), clr.g(), clr.b());
		}

		void Absorb(Color const & clr)
		{
			*(technique_->Effect().ParameterByName("absorb")) = float3(clr.r(), clr.g(), clr.b());
		}
		
		void OnRenderBegin()
		{
			App3DFramework const & app = Context::Instance().AppInstance();

			float4x4 const & view = app.ActiveCamera().ViewMatrix();
			float4x4 const & proj = app.ActiveCamera().ProjMatrix();

			*(technique_->Effect().ParameterByName("mvp")) = model_mat_ * view * proj;

			float4x4 inv_mv = MathLib::inverse(model_mat_ * view);
			*(technique_->Effect().ParameterByName("eye_pos")) = MathLib::transform_coord(float3(0, 0, 0), inv_mv);
			*(technique_->Effect().ParameterByName("look_at_vec")) = MathLib::transform_normal(float3(0, 0, 1), inv_mv);
		}
	};


	enum
	{
		Exit,
	};

	InputActionDefine actions[] =
	{
		InputActionDefine(Exit, KS_Escape),
	};
}

int main()
{
	ResLoader::Instance().AddPath("../../Samples/media/Common");

	Context::Instance().LoadCfg("KlayGE.cfg");

	AtmosphericScatteringApp app;
	app.Create();
	app.Run();
	
	return 0;
}

AtmosphericScatteringApp::AtmosphericScatteringApp()
	: App3DFramework("Atmospheric Scattering")
{
	ResLoader::Instance().AddPath("../../Samples/media/AtmosphericScattering");
}

bool AtmosphericScatteringApp::ConfirmDevice() const
{
	RenderDeviceCaps const & caps = Context::Instance().RenderFactoryInstance().RenderEngineInstance().DeviceCaps();
	if (caps.max_shader_model < 2)
	{
		return false;
	}
	return true;
}

void AtmosphericScatteringApp::InitObjects()
{
	font_ = Context::Instance().RenderFactoryInstance().MakeFont("gkai00mp.kfont");

	this->LookAt(float3(0, 0, -4.0f), float3(0, 0, 0), float3(0, 1, 0));
	this->Proj(0.01f, 500.0f);

	tb_controller_.AttachCamera(this->ActiveCamera());
	tb_controller_.Scalers(0.003f, 0.003f);

	RenderModelPtr model_planet = SyncLoadModel("geosphere.7z//geosphere.meshml", EAH_GPU_Read,
		CreateModelFactory<RenderModel>(), CreateMeshFactory<PlanetMesh>());
	planet_ = MakeSharedPtr<SceneObjectHelper>(model_planet->Mesh(0), SceneObjectHelper::SOA_Cullable);
	planet_->AddToSceneManager();

	RenderModelPtr model_atmosphere = SyncLoadModel("geosphere.7z//geosphere.meshml", EAH_GPU_Read,
		CreateModelFactory<RenderModel>(), CreateMeshFactory<AtmosphereMesh>());
	atmosphere_ = MakeSharedPtr<SceneObjectHelper>(model_atmosphere->Mesh(0), SceneObjectHelper::SOA_Cullable);
	atmosphere_->AddToSceneManager();

	UIManager::Instance().Load(ResLoader::Instance().Open("AtmosphericScattering.uiml"));
	dialog_param_ = UIManager::Instance().GetDialog("AtmosphericScattering");
	id_atmosphere_top_ = dialog_param_->IDFromName("atmosphere_top");
	id_density_ = dialog_param_->IDFromName("density");
	id_beta_button_ = dialog_param_->IDFromName("beta_button");
	id_absorb_button_ = dialog_param_->IDFromName("absorb_button");

	dialog_param_->Control<UISlider>(id_atmosphere_top_)->OnValueChangedEvent().connect(boost::bind(&AtmosphericScatteringApp::AtmosphereTopHandler, this, _1));
	this->AtmosphereTopHandler(*(dialog_param_->Control<UISlider>(id_atmosphere_top_)));

	dialog_param_->Control<UISlider>(id_density_)->OnValueChangedEvent().connect(boost::bind(&AtmosphericScatteringApp::DensityHandler, this, _1));
	this->DensityHandler(*(dialog_param_->Control<UISlider>(id_density_)));

	dialog_param_->Control<UITexButton>(id_beta_button_)->OnClickedEvent().connect(boost::bind(&AtmosphericScatteringApp::ChangeBetaHandler, this, _1));
	dialog_param_->Control<UITexButton>(id_absorb_button_)->OnClickedEvent().connect(boost::bind(&AtmosphericScatteringApp::ChangeAbsorbHandler, this, _1));

	this->LoadBeta(Color(38.05f, 82.36f, 214.65f, 1));
	this->LoadAbsorb(Color(0.75f, 0.85f, 1, 1));

	sun_light_ = MakeSharedPtr<DirectionalLightSource>();
	sun_light_->Attrib(0);
	sun_light_->Color(float3(1, 1, 1));
	sun_light_->Direction(float3(-1, 0, 0));
	sun_light_->AddToSceneManager();

	sun_light_src_ = MakeSharedPtr<SceneObjectLightSourceProxy>(sun_light_);
	checked_pointer_cast<SceneObjectLightSourceProxy>(sun_light_src_)->Scaling(0.1f, 0.1f, 0.1f);
	sun_light_src_->AddToSceneManager();

	InputEngine& inputEngine(Context::Instance().InputFactoryInstance().InputEngineInstance());
	InputActionMap actionMap;
	actionMap.AddActions(actions, actions + sizeof(actions) / sizeof(actions[0]));

	action_handler_t input_handler = MakeSharedPtr<input_signal>();
	input_handler->connect(boost::bind(&AtmosphericScatteringApp::InputHandler, this, _1, _2));
	inputEngine.ActionMap(actionMap, input_handler, true);
}

void AtmosphericScatteringApp::OnResize(KlayGE::uint32_t width, KlayGE::uint32_t height)
{
	App3DFramework::OnResize(width, height);

	UIManager::Instance().SettleCtrls(width, height);
}

void AtmosphericScatteringApp::LoadBeta(Color const & clr)
{
	RenderFactory& rf = Context::Instance().RenderFactoryInstance();

	checked_pointer_cast<PlanetMesh>(planet_->GetRenderable())->Beta(clr);
	checked_pointer_cast<AtmosphereMesh>(atmosphere_->GetRenderable())->Beta(clr);

	Color f4_clr = clr / 250.0f;
	ElementFormat fmt;
	uint32_t data = 0xFF000000;
	if (rf.RenderEngineInstance().DeviceCaps().texture_format_support(EF_ABGR8))
	{
		fmt = EF_ABGR8;
		data |= f4_clr.ABGR();
	}
	else
	{
		BOOST_ASSERT(rf.RenderEngineInstance().DeviceCaps().texture_format_support(EF_ARGB8));

		fmt = EF_ARGB8;
		data |= f4_clr.ARGB();
	}

	ElementInitData init_data;
	init_data.data = &data;
	init_data.row_pitch = 4;
	TexturePtr tex_for_button = rf.MakeTexture2D(1, 1, 1, 1, fmt, 1, 0, EAH_GPU_Read, &init_data);
	dialog_param_->Control<UITexButton>(id_beta_button_)->SetTexture(tex_for_button);
}

void AtmosphericScatteringApp::LoadAbsorb(Color const & clr)
{
	RenderFactory& rf = Context::Instance().RenderFactoryInstance();

	checked_pointer_cast<PlanetMesh>(planet_->GetRenderable())->Absorb(clr);
	checked_pointer_cast<AtmosphereMesh>(atmosphere_->GetRenderable())->Absorb(clr);

	ElementFormat fmt;
	uint32_t data = 0xFF000000;
	if (rf.RenderEngineInstance().DeviceCaps().texture_format_support(EF_ABGR8))
	{
		fmt = EF_ABGR8;
		data |= clr.ABGR();
	}
	else
	{
		BOOST_ASSERT(rf.RenderEngineInstance().DeviceCaps().texture_format_support(EF_ARGB8));

		fmt = EF_ARGB8;
		data |= clr.ARGB();
	}

	ElementInitData init_data;
	init_data.data = &data;
	init_data.row_pitch = 4;
	TexturePtr tex_for_button = rf.MakeTexture2D(1, 1, 1, 1, fmt, 1, 0, EAH_GPU_Read, &init_data);
	dialog_param_->Control<UITexButton>(id_absorb_button_)->SetTexture(tex_for_button);
}

void AtmosphericScatteringApp::InputHandler(KlayGE::InputEngine const & /*sender*/, KlayGE::InputAction const & action)
{
	switch (action.first)
	{
	case Exit:
		this->Quit();
		break;
	}
}

void AtmosphericScatteringApp::AtmosphereTopHandler(KlayGE::UISlider const & sender)
{
	float value = 1 + sender.GetValue() / 1000.0f;
	checked_pointer_cast<AtmosphereMesh>(atmosphere_->GetRenderable())->AtmosphereTop(value);
}

void AtmosphericScatteringApp::DensityHandler(KlayGE::UISlider const & sender)
{
	float value = sender.GetValue() / 100000.0f;
	checked_pointer_cast<PlanetMesh>(planet_->GetRenderable())->Density(value);
	checked_pointer_cast<AtmosphereMesh>(atmosphere_->GetRenderable())->Density(value);
}

void AtmosphericScatteringApp::ChangeBetaHandler(KlayGE::UITexButton const & /*sender*/)
{
#if defined KLAYGE_PLATFORM_WINDOWS
	CHOOSECOLORA occ;
	HWND hwnd = this->MainWnd()->HWnd();

	static COLORREF cust_clrs[16] = { RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF),
		RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF),
		RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF),
		RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF) };

	Color f4_clr = beta_ / 250.0f;

	ZeroMemory(&occ, sizeof(occ));
	occ.lStructSize = sizeof(occ);
	occ.hwndOwner = hwnd;
	occ.hInstance = NULL;
	occ.rgbResult = f4_clr.ABGR();
	occ.lpCustColors = cust_clrs;
	occ.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	occ.lCustData = 0;
	occ.lpfnHook = NULL;
	occ.lpTemplateName = NULL;

	if (ChooseColorA(&occ))
	{
		beta_ = Color(occ.rgbResult) * 250.0f;
		std::swap(beta_.r(), beta_.b());
		this->LoadBeta(beta_);
	}
#endif
}

void AtmosphericScatteringApp::ChangeAbsorbHandler(KlayGE::UITexButton const & /*sender*/)
{
#if defined KLAYGE_PLATFORM_WINDOWS
	CHOOSECOLORA occ;
	HWND hwnd = this->MainWnd()->HWnd();

	static COLORREF cust_clrs[16] = { RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF),
		RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF),
		RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF),
		RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF) };

	ZeroMemory(&occ, sizeof(occ));
	occ.lStructSize = sizeof(occ);
	occ.hwndOwner = hwnd;
	occ.hInstance = NULL;
	occ.rgbResult = absorb_.ABGR();
	occ.lpCustColors = cust_clrs;
	occ.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	occ.lCustData = 0;
	occ.lpfnHook = NULL;
	occ.lpTemplateName = NULL;

	if (ChooseColorA(&occ))
	{
		absorb_ = Color(occ.rgbResult);
		std::swap(absorb_.r(), absorb_.b());
		this->LoadAbsorb(absorb_);
	}
#endif
}

void AtmosphericScatteringApp::DoUpdateOverlay()
{
	UIManager::Instance().Render();

	font_->RenderText(0, 0, Color(1, 1, 0, 1), L"Atmospheric Scattering", 16);

	std::wostringstream stream;
	stream.precision(2);
	stream << std::fixed << this->FPS() << " FPS";
	font_->RenderText(0, 18, Color(1, 1, 0, 1), stream.str(), 16);
}

uint32_t AtmosphericScatteringApp::DoUpdate(KlayGE::uint32_t /*pass*/)
{
	RenderEngine& re = Context::Instance().RenderFactoryInstance().RenderEngineInstance();

	re.BindFrameBuffer(FrameBufferPtr());
	Color clear_clr(0.0f, 0.0f, 0.0f, 1);

	re.CurFrameBuffer()->Clear(FrameBuffer::CBM_Color | FrameBuffer::CBM_Depth | FrameBuffer::CBM_Stencil, clear_clr, 1, 0);
	return App3DFramework::URV_Need_Flush | App3DFramework::URV_Finished;
}