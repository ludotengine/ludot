<script setup lang="ts">
import { computed, ref } from 'vue'
import { withBase } from 'vitepress'
import classes from '../../../.generated/classes.json'
const query = ref('')
const category = ref('All classes')
const categories = [
	'All classes',
	'2D',
	'3D',
	'UI',
	'Resources',
	'Value types',
	'Editor',
	'Other',
]
const filtered = computed(() =>
	classes.filter(
		(entry) =>
			(category.value === 'All classes' ||
				entry.category === category.value) &&
			`${entry.name} ${entry.brief}`
				.toLowerCase()
				.includes(query.value.toLowerCase()),
	),
)
</script>

<template>
	<div class="api-browser">
		<label class="api-search"
			>Find an engine class<input
				v-model="query"
				type="search"
				placeholder="Node, Vector3, CharacterBody…"
		/></label>
		<div class="api-categories" aria-label="Filter class category">
			<button
				v-for="item in categories"
				:key="item"
				:class="{ active: category === item }"
				:aria-pressed="category === item"
				@click="category = item"
			>
				{{ item }}
			</button>
		</div>
		<p class="api-count" role="status">
			{{ filtered.length }} classes · Default Luau naming
		</p>
		<div class="api-list">
			<a
				v-for="entry in filtered"
				:key="entry.name"
				:href="withBase(`/docs/api/engine/${entry.name}.html`)"
				><strong>{{ entry.name }}</strong
				><span>{{
					entry.brief ||
					`Inherits ${entry.parent || 'native engine type'}.`
				}}</span
				><small>{{ entry.category }}</small></a
			>
		</div>
		<p v-if="!filtered.length">
			No matching classes. Try a shorter name or another category.
		</p>
	</div>
</template>
