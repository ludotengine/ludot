<script setup lang="ts">
import { onMounted, ref } from 'vue'
const props = defineProps<{ suffix: string }>()
const url = ref('https://github.com/ludotengine/ludot/releases/latest')
onMounted(async () => {
	try {
		const response = await fetch(
			'https://api.github.com/repos/ludotengine/ludot/releases/latest',
		)
		if (!response.ok) return
		const release = await response.json()
		const asset = release.assets.find((item: { name: string }) =>
			item.name.endsWith(props.suffix),
		)
		if (asset) url.value = asset.browser_download_url
	} catch {
		/* The release page remains available if API access fails. */
	}
})
</script>

<template>
	<a class="primary-button" :href="url"><slot /></a>
</template>
