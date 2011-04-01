#ifndef _PARALLAX_HPP
#define _PARALLAX_HPP

#include <KlayGE/App3D.hpp>
#include <KlayGE/Font.hpp>
#include <KlayGE/CameraController.hpp>
#include <KlayGE/UI.hpp>
#include <KlayGE/PostProcess.hpp>

#include "DeferredRenderingLayerAdv.hpp"

class GlobalIlluminationApp : public KlayGE::App3DFramework
{
public:
	GlobalIlluminationApp();

	bool ConfirmDevice() const;

private:
	void InitObjects();
	void OnResize(KlayGE::uint32_t width, KlayGE::uint32_t height);

	void DoUpdateOverlay();
	KlayGE::uint32_t DoUpdate(KlayGE::uint32_t pass);

	void InputHandler(KlayGE::InputEngine const & sender, KlayGE::InputAction const & action);
	void IllumChangedHandler(KlayGE::UIComboBox const & sender);
	void CtrlCameraHandler(KlayGE::UICheckBox const & sender);

	KlayGE::FontPtr font_;
	KlayGE::RenderModelPtr scene_model_;
	std::vector<KlayGE::SceneObjectPtr> scene_objs_;
	KlayGE::SceneObjectPtr sky_box_;
	KlayGE::SceneObjectPtr spot_light_src_;

	KlayGE::AmbientLightSourcePtr ambient_light_;
	KlayGE::SpotLightSourcePtr spot_light_;

	KlayGE::FirstPersonCameraController fpcController_;

	KlayGE::DeferredRenderingLayerPtr deferred_rendering_;

	KlayGE::PostProcessPtr copy_pp_;

	KlayGE::UIDialogPtr dialog_;

	int id_illum_combo_;
	int id_ctrl_camera_;
};

#endif		// _PARALLAX_HPP