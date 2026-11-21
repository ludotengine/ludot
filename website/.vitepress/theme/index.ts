import type { Theme } from 'vitepress'
import Layout from './Layout.vue'
import Playground from './components/Playground.vue'
import ApiIndex from './components/ApiIndex.vue'
import InstallOptions from './components/InstallOptions.vue'
import ReleaseDownload from './components/ReleaseDownload.vue'
import InheritedMembers from './components/InheritedMembers.vue'
import Home from './components/Home.vue'
import 'vitepress/dist/client/theme-default/styles/vars.css'
import 'vitepress/dist/client/theme-default/styles/base.css'
import 'vitepress/dist/client/theme-default/styles/icons.css'
import 'vitepress/dist/client/theme-default/styles/utils.css'
import 'vitepress/dist/client/theme-default/styles/components/vp-doc.css'
import 'vitepress/dist/client/theme-default/styles/components/custom-block.css'
import 'vitepress/dist/client/theme-default/styles/components/vp-code.css'
import './style.css'

export default {
	Layout,
	enhanceApp({ app }) {
		app.component('Playground', Playground)
		app.component('ApiIndex', ApiIndex)
		app.component('Home', Home)
		app.component('InheritedMembers', InheritedMembers)
		app.component('InstallOptions', InstallOptions)
		app.component('ReleaseDownload', ReleaseDownload)
	},
} satisfies Theme
