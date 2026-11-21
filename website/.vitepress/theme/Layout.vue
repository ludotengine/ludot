<script setup lang="ts">
import { computed, nextTick, onMounted, onUnmounted, ref, watch } from 'vue'
import { useData, useRoute, withBase } from 'vitepress'
import { navigation, href } from './navigation'
import Search from './components/Search.vue'
import ReferenceNav from './components/ReferenceNav.vue'
import {
	PhArrowUp,
	PhFunnel,
	PhGithubLogo,
	PhList,
	PhMagnifyingGlass,
	PhMoon,
	PhSun,
	PhX,
} from '@phosphor-icons/vue'

const { frontmatter, page, isDark } = useData()
const route = useRoute()
const filter = ref('')
const mobileOpen = ref(false)
const searchOpen = ref(false)
const headings = ref<{ id: string; title: string; member: boolean }[]>([])
const isReference = computed(() => route.path.includes('/docs/api/'))
const isHome = computed(() => frontmatter.value.layout === 'home')
const groups = computed(() =>
	navigation
		.map((group) => ({
			...group,
			items: group.items.filter(([title]) =>
				title.toLowerCase().includes(filter.value.toLowerCase()),
			),
		}))
		.filter((group) => group.items.length),
)
const section = computed(() => {
	if (route.path.includes('/api/')) return 'Script reference'
	if (route.path.includes('/luau/')) return 'Luau language'
	if (route.path.includes('/engine/')) return 'Engine manual'
	return 'User manual'
})
const currentIndex = computed(() =>
	navigation
		.flatMap((group) => group.items)
		.findIndex(([, path]) => active(path)),
)
const adjacent = computed(() => {
	const all = navigation.flatMap((group) => group.items)
	return {
		previous: all[currentIndex.value - 1],
		next: all[currentIndex.value + 1],
	}
})
function active(path: string) {
	return route.path === withBase(href(path)) || route.path === withBase(path)
}
function keydown(event: KeyboardEvent) {
	if ((event.metaKey || event.ctrlKey) && event.key.toLowerCase() === 'k') {
		event.preventDefault()
		searchOpen.value = !searchOpen.value
	}
	if (event.key === 'Escape') {
		searchOpen.value = false
		mobileOpen.value = false
	}
}
function jumpToMember(event: Event) {
	const id = (event.target as HTMLSelectElement).value
	if (id) {
		document.getElementById(id)?.scrollIntoView()
		history.replaceState(null, '', `#${id}`)
	}
}
function editSnippet(event: MouseEvent) {
	const link = (event.target as HTMLElement).closest('.snippet-edit')
	if (!link) return
	const code = link.parentElement?.querySelector('code')?.textContent || ''
	sessionStorage.setItem(
		'ludot:playground',
		JSON.stringify({
			code,
			engine: isReference.value || route.path.includes('/engine/'),
		}),
	)
}
function updateOutline() {
	for (const block of document.querySelectorAll(
		'article .language-luau, article .language-lua',
	)) {
		if (block.querySelector('.snippet-edit')) continue
		const edit = document.createElement('a')
		edit.className = 'snippet-edit'
		edit.textContent = 'Edit code'
		edit.href = withBase('/docs/playground.html')
		block.append(edit)
	}
	headings.value = Array.from(
		document.querySelectorAll(
			isReference.value
				? 'article h2, article h3[data-member]'
				: 'article h2',
		),
	).map((element) => ({
		id: element.id,
		title:
			element.textContent?.replace(/[\s\u200b]*#?[\s\u200b]*$/, '') || '',
		member: element.hasAttribute('data-member'),
	}))
}
let observer: MutationObserver
onMounted(() => {
	document.addEventListener('keydown', keydown)
	document.addEventListener('click', editSnippet)
	observer = new MutationObserver(updateOutline)
	observer.observe(document.querySelector('main')!, {
		childList: true,
		subtree: true,
	})
	updateOutline()
})
onUnmounted(() => {
	document.removeEventListener('keydown', keydown)
	document.removeEventListener('click', editSnippet)
	observer?.disconnect()
})
watch(
	() => route.path,
	async () => {
		mobileOpen.value = false
		await nextTick()
		observer?.disconnect()
		const main = document.querySelector('main')
		if (main && observer)
			observer.observe(main, { childList: true, subtree: true })
		updateOutline()
	},
)
</script>

<template>
	<a class="skip-link" href="#main-content">Skip to content</a>
	<header class="site-header">
		<div class="topbar">
			<a class="brand" :href="withBase('/')"
				><img :src="withBase('/logo.svg')" alt="" /><span
					>Ludot</span
				></a
			>
			<span class="header-divider"></span>
			<a class="header-label" :href="withBase('/docs/')">Documentation</a>
			<nav class="global-links" aria-label="Main navigation">
				<a :href="withBase('/docs/luau/')">Language</a>
				<a :href="withBase('/docs/engine/')"
					>Engine <span class="tiny-badge">Experimental</span></a
				>
				<a :href="withBase('/docs/playground.html')">Playground</a>
			</nav>
			<div class="header-actions">
				<button class="search-trigger" @click="searchOpen = true">
					<PhMagnifyingGlass :size="16" aria-hidden="true" /><span
						>Search documentation</span
					><kbd>Ctrl K</kbd>
				</button>
				<button
					class="theme-toggle"
					:aria-label="isDark ? 'Use light theme' : 'Use dark theme'"
					@click="isDark = !isDark"
				>
					<PhSun v-if="isDark" :size="20" aria-hidden="true" />
					<PhMoon v-else :size="20" aria-hidden="true" />
				</button>
				<a
					class="github-link"
					href="https://github.com/ludotengine/ludot"
					aria-label="Ludot on GitHub"
					><PhGithubLogo :size="20" aria-hidden="true"
				/></a>
			</div>
		</div>
		<div v-if="!isHome" class="subbar">
			<button
				class="mobile-menu"
				:aria-expanded="mobileOpen"
				aria-controls="docs-sidebar"
				@click="mobileOpen = !mobileOpen"
			>
				<PhList :size="16" aria-hidden="true" /> <span>Contents</span>
			</button>
			<a class="product-label" :href="withBase('/docs/')"
				>Ludot documentation</a
			>
			<span class="latest-label">Latest</span>
			<nav aria-label="Documentation areas">
				<a
					:class="{ selected: !route.path.includes('/api/') }"
					:href="withBase('/docs/')"
					>User manual</a
				>
				<a
					:class="{ selected: route.path.includes('/api/') }"
					:href="withBase('/docs/api/')"
					>Script reference</a
				>
			</nav>
			<a
				class="get-ludot"
				:href="withBase('/docs/guide/installation.html')"
				>Get Ludot</a
			>
		</div>
	</header>

	<main v-if="isHome" id="main-content" class="home-main"><Content /></main>
	<template v-else>
		<button
			v-if="mobileOpen"
			class="sidebar-overlay"
			aria-label="Close contents"
			@click="mobileOpen = false"
		></button>
		<aside
			id="docs-sidebar"
			class="docs-sidebar"
			:class="{ open: mobileOpen }"
			aria-label="Documentation navigation"
		>
			<ReferenceNav v-if="isReference" />
			<template v-else>
				<a class="sidebar-home" :href="withBase('/docs/')"
					>Ludot documentation</a
				>
				<label class="sidebar-filter"
					><PhFunnel :size="15" aria-hidden="true" /><input
						v-model="filter"
						aria-label="Filter navigation by title"
						placeholder="Filter by title" /><button
						v-if="filter"
						aria-label="Clear filter"
						@click="filter = ''"
					>
						<PhX :size="14" aria-hidden="true" /></button
				></label>
				<nav>
					<section
						v-for="group in groups"
						:key="group.title"
						class="nav-group"
					>
						<h2>{{ group.title }}</h2>
						<a
							v-for="[title, path] in group.items"
							:key="path"
							:href="withBase(href(path))"
							:class="{ active: active(path) }"
							:aria-current="active(path) ? 'page' : undefined"
							>{{ title }}</a
						>
					</section>
					<p v-if="!groups.length" class="muted">
						No matching titles.
					</p>
				</nav>
			</template>
			<div class="sidebar-footer">Built with Luau. Based on Godot.</div>
		</aside>
		<main id="main-content" class="docs-main">
			<div class="article-wrap">
				<div class="breadcrumb">
					<a :href="withBase('/docs/')">Documentation</a><span>/</span
					><span>{{ section }}</span>
				</div>
				<div
					v-if="route.path.includes('/engine/') || frontmatter.api"
					class="experimental-notice"
				>
					<span>Experimental</span> The engine changes frequently.
					Breaking changes are expected.
				</div>
				<article class="vp-doc"><Content /></article>
				<div v-if="page.isNotFound" class="not-found">
					<h1>Page not found</h1>
					<p>This page may have moved as Ludot changed.</p>
					<a :href="withBase('/docs/')">Browse the documentation</a>
				</div>
				<footer class="article-footer">
					<div class="page-tools">
						<a
							:href="`https://github.com/ludotengine/ludot/blob/main/${frontmatter.source || 'website/' + page.relativePath}`"
							>View page source</a
						><a href="#main-content"
							>Back to top
							<PhArrowUp :size="13" aria-hidden="true"
						/></a>
					</div>
					<div class="adjacent-pages">
						<a
							v-if="adjacent.previous"
							:href="withBase(href(adjacent.previous[1]))"
							><small>Previous</small
							>{{ adjacent.previous[0] }}</a
						>
						<a
							v-if="adjacent.next"
							:href="withBase(href(adjacent.next[1]))"
							><small>Next</small>{{ adjacent.next[0] }}</a
						>
					</div>
				</footer>
			</div>
			<aside class="page-outline" aria-label="On this page">
				<div class="outline-sticky">
					<label v-if="isReference" class="reference-jump"
						>Jump to
						<select
							aria-label="Jump to API member"
							@change="jumpToMember"
						>
							<option value="">Choose a section or member</option>
							<option
								v-for="heading in headings"
								:key="heading.id"
								:value="heading.id"
							>
								{{ heading.title }}
							</option>
						</select>
					</label>
					<h2>On this page</h2>
					<a
						v-for="heading in headings"
						:key="heading.id"
						:href="`#${heading.id}`"
						:class="{ 'outline-member': heading.member }"
						>{{ heading.title }}</a
					>
					<div class="outline-bottom">
						<p>Help improve these docs</p>
						<a
							href="https://github.com/ludotengine/ludot/issues/new"
							>Report an issue</a
						><span>Always the latest revision.</span>
					</div>
				</div>
			</aside>
		</main>
	</template>
	<Search v-if="searchOpen" @close="searchOpen = false" />
</template>
